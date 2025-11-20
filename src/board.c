#include <string.h>

#include "board.h"

void board_reset(Board *board) {
    if (board == NULL) {
        return;
    }

    memset(board->cells, 0, sizeof(board->cells));
}

bool board_can_place(const Board *board,
                     const PieceShape *shape,
                     int rotation,
                     int test_row,
                     int test_col) {
    if (board == NULL || shape == NULL) {
        return false;
    }

    const char *pattern = shape->rotations[rotation];

    for (int r = 0; r < shape->size; ++r) {
        for (int c = 0; c < shape->size; ++c) {
            if (pattern[r * shape->size + c] != '1') {
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

            if (board->cells[board_row][board_col] != 0) {
                return false;
            }
        }
    }

    return true;
}

void board_lock_shape(Board *board,
                      const PieceShape *shape,
                      int rotation,
                      int base_row,
                      int base_col,
                      int value) {
    if (board == NULL || shape == NULL) {
        return;
    }

    const char *pattern = shape->rotations[rotation];

    for (int r = 0; r < shape->size; ++r) {
        for (int c = 0; c < shape->size; ++c) {
            if (pattern[r * shape->size + c] != '1') {
                continue;
            }

            int board_row = base_row + r;
            int board_col = base_col + c;

            if (board_row < 0 || board_row >= BOARD_HEIGHT || board_col < 0 || board_col >= BOARD_WIDTH) {
                continue;
            }

            board->cells[board_row][board_col] = value;
        }
    }
}

int board_clear_completed_lines(Board *board, int *rows_out, int max_rows) {
    if (board == NULL) {
        return 0;
    }

    int cleared = 0;

    for (int row = BOARD_HEIGHT - 1; row >= 0; --row) {
        bool full = true;
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            if (board->cells[row][col] == 0) {
                full = false;
                break;
            }
        }

        if (!full) {
            continue;
        }

        if (rows_out != NULL && cleared < max_rows) {
            rows_out[cleared] = row;
        }

        for (int move_row = row; move_row > 0; --move_row) {
            memcpy(board->cells[move_row], board->cells[move_row - 1], sizeof(board->cells[move_row]));
        }
        memset(board->cells[0], 0, sizeof(board->cells[0]));
        ++cleared;
        ++row;
    }

    return cleared;
}
