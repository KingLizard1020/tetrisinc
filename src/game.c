#if defined(_WIN32)
#include <curses.h>
#else
#include <ncurses.h>
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define CELL_EMPTY 0

#define GRAVITY_INTERVAL_MS 700ULL

typedef struct {
    int size;
    int rotation_count;
    const char *rotations[4];
} PieceDef;

typedef struct {
    int type;
    int rotation;
    int row;
    int col;
    bool active;
} ActivePiece;

static const PieceDef g_piece_defs[] = {
    {4, 2, {"0000111100000000", "0010001000100010"}},              /* I */
    {4, 1, {"0011001100000000"}},                                  /* O */
    {4, 4, {"0000010011100000", "0010011000100000", "0000111001000000", "0100011001000000"}}, /* T */
    {4, 4, {"0000001000100011", "0000111001000000", "0011001000100000", "0000010011100000"}}, /* L */
    {4, 4, {"0000001000100110", "0000111000100000", "0011010001000000", "0010001110000000"}}, /* J */
    {4, 2, {"0000011001100000", "0010011000100000"}},              /* S */
    {4, 2, {"0000110001100000", "0000011001000010"}}               /* Z */
};

static const size_t PIECE_COUNT = sizeof(g_piece_defs) / sizeof(g_piece_defs[0]);

static bool g_use_color = false;
static int g_board[BOARD_HEIGHT][BOARD_WIDTH];
static ActivePiece g_active_piece;
static uint64_t g_gravity_accumulator_ms = 0ULL;

static void board_reset(void);
static void spawn_piece(void);
static bool try_move_piece(int drow, int dcol);
static bool try_rotate_piece(int direction);
static bool can_place_piece(int test_row, int test_col, int rotation);
static void lock_piece(void);
static void clear_completed_lines(void);
static void handle_input(int ch, bool *running);
static void update_game(uint64_t delta_ms);
static uint64_t monotonic_millis(void);
static void draw_frame(void);
static bool has_enough_space(void);
static void draw_banner(void);
static void draw_board(int origin_y, int origin_x);
static void draw_active_piece(int origin_y, int origin_x);
static void draw_piece_cells(int screen_y, int screen_x);

int game_init(void) {
    if (initscr() == NULL) {
        return -1;
    }

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_CYAN, -1);
        g_use_color = true;
    }

    board_reset();
    srand((unsigned int)time(NULL));
    spawn_piece();

    return 0;
}

void game_loop(void) {
    bool running = true;
    uint64_t last_tick = monotonic_millis();

    while (running) {
        int ch = getch();
        handle_input(ch, &running);

        uint64_t now = monotonic_millis();
        uint64_t delta = now - last_tick;
        last_tick = now;

        update_game(delta);

        draw_frame();
        napms(1);
    }
}

void game_shutdown(void) {
    endwin();
}

static void draw_frame(void) {
    erase();
    box(stdscr, 0, 0);

    if (!has_enough_space()) {
        mvprintw(LINES / 2, (COLS - 30) / 2, "Enlarge the terminal window.");
        refresh();
        return;
    }

    draw_banner();
    draw_board(4, 8);
    draw_active_piece(4, 8);

    refresh();
}

static void board_reset(void) {
    memset(g_board, 0, sizeof(g_board));
    g_active_piece.active = false;
    g_gravity_accumulator_ms = 0ULL;
}

static bool has_enough_space(void) {
    const int min_rows = BOARD_HEIGHT + 8;
    const int min_cols = BOARD_WIDTH * 2 + 18;
    return (LINES >= min_rows) && (COLS >= min_cols);
}

static void draw_banner(void) {
    if (g_use_color) {
        attron(COLOR_PAIR(1));
    }
    mvprintw(1, 2, "Terminal Tetris Prototype");
    if (g_use_color) {
        attroff(COLOR_PAIR(1));
    }

    mvprintw(2, 2, "Press 'q' to quit");
    mvprintw(3, 2, "Pieces fall automatically. Arrows move.");
}

static void draw_board(int origin_y, int origin_x) {
    const int inner_width = BOARD_WIDTH * 2;

    move(origin_y - 1, origin_x - 1);
    addch('+');
    for (int i = 0; i < inner_width; ++i) {
        addch('-');
    }
    addch('+');

    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        move(origin_y + row, origin_x - 1);
        addch('|');
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            if (g_board[row][col] != CELL_EMPTY) {
                if (g_use_color) {
                    attron(COLOR_PAIR(1));
                }
                addstr("[]");
                if (g_use_color) {
                    attroff(COLOR_PAIR(1));
                }
            } else {
                addstr("  ");
            }
        }
        addch('|');
    }

    move(origin_y + BOARD_HEIGHT, origin_x - 1);
    addch('+');
    for (int i = 0; i < inner_width; ++i) {
        addch('-');
    }
    addch('+');
}

static void draw_active_piece(int origin_y, int origin_x) {
    if (!g_active_piece.active) {
        return;
    }

    const PieceDef *def = &g_piece_defs[g_active_piece.type];
    const char *pattern = def->rotations[g_active_piece.rotation];

    for (int r = 0; r < def->size; ++r) {
        for (int c = 0; c < def->size; ++c) {
            if (pattern[r * def->size + c] != '1') {
                continue;
            }

            int board_row = g_active_piece.row + r;
            int board_col = g_active_piece.col + c;
            if (board_row < 0 || board_row >= BOARD_HEIGHT || board_col < 0 || board_col >= BOARD_WIDTH) {
                continue;
            }

            int screen_y = origin_y + board_row;
            int screen_x = origin_x + board_col * 2;

            move(screen_y, screen_x);
            if (g_use_color) {
                attron(COLOR_PAIR(1));
            }
            addstr("[]");
            if (g_use_color) {
                attroff(COLOR_PAIR(1));
            }
        }
    }
}

static void handle_input(int ch, bool *running) {
    if (ch == ERR) {
        return;
    }

    switch (ch) {
        case 'q':
        case 'Q':
            *running = false;
            break;
        case KEY_LEFT:
        case 'a':
        case 'A':
            try_move_piece(0, -1);
            break;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            try_move_piece(0, 1);
            break;
        case KEY_DOWN:
        case 's':
        case 'S':
            if (!try_move_piece(1, 0)) {
                lock_piece();
                clear_completed_lines();
                spawn_piece();
            }
            break;
        case KEY_UP:
        case 'w':
        case 'W':
            try_rotate_piece(1);
            break;
    }
}

static void update_game(uint64_t delta_ms) {
    if (!g_active_piece.active) {
        spawn_piece();
    }

    g_gravity_accumulator_ms += delta_ms;
    while (g_gravity_accumulator_ms >= GRAVITY_INTERVAL_MS) {
        g_gravity_accumulator_ms -= GRAVITY_INTERVAL_MS;
        if (!try_move_piece(1, 0)) {
            lock_piece();
            clear_completed_lines();
            spawn_piece();
            break;
        }
    }
}

static uint64_t monotonic_millis(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000ULL);
}

static const PieceDef *current_piece_def(void) {
    return &g_piece_defs[g_active_piece.type];
}

static void spawn_piece(void) {
    g_active_piece.type = (int)(rand() % (int)PIECE_COUNT);
    g_active_piece.rotation = 0;
    g_active_piece.row = -2;
    const PieceDef *def = current_piece_def();
    g_active_piece.col = (BOARD_WIDTH - def->size) / 2;
    g_active_piece.active = true;

    if (!can_place_piece(g_active_piece.row, g_active_piece.col, g_active_piece.rotation)) {
        board_reset();
        g_active_piece.active = false;
    }
}

static bool try_move_piece(int drow, int dcol) {
    if (!g_active_piece.active) {
        return false;
    }

    int next_row = g_active_piece.row + drow;
    int next_col = g_active_piece.col + dcol;
    if (!can_place_piece(next_row, next_col, g_active_piece.rotation)) {
        return false;
    }

    g_active_piece.row = next_row;
    g_active_piece.col = next_col;
    return true;
}

static bool try_rotate_piece(int direction) {
    if (!g_active_piece.active) {
        return false;
    }

    const PieceDef *def = current_piece_def();
    int next_rotation = (g_active_piece.rotation + direction + def->rotation_count) % def->rotation_count;
    if (!can_place_piece(g_active_piece.row, g_active_piece.col, next_rotation)) {
        return false;
    }

    g_active_piece.rotation = next_rotation;
    return true;
}

static bool can_place_piece(int test_row, int test_col, int rotation) {
    const PieceDef *def = &g_piece_defs[g_active_piece.type];
    const char *pattern = def->rotations[rotation];

    for (int r = 0; r < def->size; ++r) {
        for (int c = 0; c < def->size; ++c) {
            if (pattern[r * def->size + c] != '1') {
                continue;
            }

            int board_row = test_row + r;
            int board_col = test_col + c;

            if (board_row < 0) {
                continue;
            }

            if (board_col < 0 || board_col >= BOARD_WIDTH || board_row >= BOARD_HEIGHT) {
                return false;
            }

            if (g_board[board_row][board_col] != CELL_EMPTY) {
                return false;
            }
        }
    }

    return true;
}

static void lock_piece(void) {
    if (!g_active_piece.active) {
        return;
    }

    const PieceDef *def = current_piece_def();
    const char *pattern = def->rotations[g_active_piece.rotation];

    for (int r = 0; r < def->size; ++r) {
        for (int c = 0; c < def->size; ++c) {
            if (pattern[r * def->size + c] != '1') {
                continue;
            }

            int board_row = g_active_piece.row + r;
            int board_col = g_active_piece.col + c;

            if (board_row < 0 || board_row >= BOARD_HEIGHT || board_col < 0 || board_col >= BOARD_WIDTH) {
                continue;
            }

            g_board[board_row][board_col] = g_active_piece.type + 1;
        }
    }

    g_active_piece.active = false;
}

static void clear_completed_lines(void) {
    for (int row = BOARD_HEIGHT - 1; row >= 0; --row) {
        bool full = true;
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            if (g_board[row][col] == CELL_EMPTY) {
                full = false;
                break;
            }
        }

        if (!full) {
            continue;
        }

        for (int move_row = row; move_row > 0; --move_row) {
            memcpy(g_board[move_row], g_board[move_row - 1], sizeof(g_board[move_row]));
        }
        memset(g_board[0], 0, sizeof(g_board[0]));
        ++row; /* recheck same row after collapsing */
    }
}
