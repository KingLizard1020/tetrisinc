#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "piece.h"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

typedef struct {
    int cells[BOARD_HEIGHT][BOARD_WIDTH];
} Board;

void board_reset(Board *board);

bool board_can_place(const Board *board,
                     const PieceShape *shape,
                     int rotation,
                     int test_row,
                     int test_col);

void board_lock_shape(Board *board,
                      const PieceShape *shape,
                      int rotation,
                      int base_row,
                      int base_col,
                      int value);

int board_clear_completed_lines(Board *board, int *rows_out, int max_rows);

#endif /* BOARD_H */
