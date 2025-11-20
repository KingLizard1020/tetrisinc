#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "piece.h"

static const PieceShape *first_shape(void) {
    if (piece_shape_count() == 0) {
        return NULL;
    }
    return piece_shape_get(0);
}

static void test_board_can_place_empty(void) {
    Board board;
    board_reset(&board);
    const PieceShape *shape = first_shape();
    assert(shape != NULL);
    assert(board_can_place(&board, shape, 0, 0, 3));
}

static void test_board_can_place_blocked(void) {
    Board board;
    board_reset(&board);
    const PieceShape *shape = first_shape();
    assert(shape != NULL);

    board.cells[1][3] = 99;
    assert(!board_can_place(&board, shape, 0, 0, 3));
}

static void test_board_lock_and_clear(void) {
    Board board;
    board_reset(&board);
    const PieceShape *shape = first_shape();
    assert(shape != NULL);

    board_lock_shape(&board, shape, 0, 0, 3, 1);
    assert(board.cells[1][3] == 1);

    for (int col = 0; col < BOARD_WIDTH; ++col) {
        board.cells[BOARD_HEIGHT - 1][col] = 5;
    }
    int rows[BOARD_HEIGHT];
    int cleared = board_clear_completed_lines(&board, rows, BOARD_HEIGHT);
    assert(cleared == 1);
    for (int col = 0; col < BOARD_WIDTH; ++col) {
        assert(board.cells[BOARD_HEIGHT - 1][col] == 0);
    }
}

static void test_board_can_place_left_boundary(void) {
    Board board;
    board_reset(&board);
    const PieceShape *shape = first_shape();
    assert(shape != NULL);

    assert(!board_can_place(&board, shape, 0, 0, -1));
}

static void test_board_can_place_above_board(void) {
    Board board;
    board_reset(&board);
    const PieceShape *shape = first_shape();
    assert(shape != NULL);

    assert(board_can_place(&board, shape, 0, -3, 3));
}

static void test_board_lock_ignores_out_of_bounds_cells(void) {
    Board board;
    board_reset(&board);
    const PieceShape *shape = first_shape();
    assert(shape != NULL);

    board_lock_shape(&board, shape, 0, -2, 4, 7);

    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            assert(board.cells[row][col] == 0 || board.cells[row][col] == 7);
        }
    }
}

static void test_board_clear_multiple_lines(void) {
    Board board;
    board_reset(&board);

    for (int col = 0; col < BOARD_WIDTH; ++col) {
        board.cells[BOARD_HEIGHT - 1][col] = 1;
        board.cells[BOARD_HEIGHT - 2][col] = 2;
    }

    int rows[BOARD_HEIGHT];
    int cleared = board_clear_completed_lines(&board, rows, BOARD_HEIGHT);
    assert(cleared == 2);

    for (int row = BOARD_HEIGHT - 2; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            assert(board.cells[row][col] == 0);
        }
    }
}

static void run_test(const char *name, void (*fn)(void)) {
    printf("[RUN] %s\n", name);
    fn();
    printf("[OK ] %s\n", name);
}

int main(void) {
    run_test("board_can_place_empty", test_board_can_place_empty);
    run_test("board_can_place_blocked", test_board_can_place_blocked);
    run_test("board_lock_and_clear", test_board_lock_and_clear);
    run_test("board_can_place_left_boundary", test_board_can_place_left_boundary);
    run_test("board_can_place_above_board", test_board_can_place_above_board);
    run_test("board_lock_ignores_out_of_bounds_cells", test_board_lock_ignores_out_of_bounds_cells);
    run_test("board_clear_multiple_lines", test_board_clear_multiple_lines);
    return 0;
}
