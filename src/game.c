#include <ncurses.h>
#include <stdbool.h>

#include "game.h"

static bool g_use_color = false;
static void draw_frame(void);

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

    return 0;
}

void game_loop(void) {
    bool running = true;

    while (running) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = false;
        }

        draw_frame();
        napms(16); /* ~60 FPS placeholder */
    }
}

void game_shutdown(void) {
    endwin();
}

static void draw_frame(void) {
    erase();
    box(stdscr, 0, 0);

    if (g_use_color) {
        attron(COLOR_PAIR(1));
    }
    mvprintw(1, 2, "Terminal Tetris Prototype");
    if (g_use_color) {
        attroff(COLOR_PAIR(1));
    }

    mvprintw(3, 2, "Press 'q' to quit");
    mvprintw(5, 2, "Game loop + input stub running...");

    refresh();
}
