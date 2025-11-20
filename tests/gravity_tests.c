#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "board.h"
#include "piece.h"

#define SIM_LOCK_THRESHOLD 3

typedef struct {
    Board board;
    ActivePiece piece;
    int lock_ticks;
    bool locked;
} SimState;

static void run_test(const char *name, void (*fn)(void)) {
    printf("[RUN] %s\n", name);
    fn();
    printf("[OK ] %s\n", name);
}

static void sim_init(SimState *sim, int piece_type) {
    board_reset(&sim->board);
    sim->lock_ticks = 0;
    sim->locked = false;

    int type = piece_type % (int)piece_shape_count();
    const PieceShape *shape = piece_shape_get((size_t)type);
    assert(shape != NULL);

    sim->piece.type = type;
    sim->piece.rotation = 0;
    sim->piece.row = -2;
    sim->piece.col = (BOARD_WIDTH - shape->size) / 2;
    sim->piece.active = true;
}

static bool sim_try_move(SimState *sim, int drow, int dcol) {
    if (sim->locked) {
        return false;
    }

    const PieceShape *shape = piece_shape_get((size_t)sim->piece.type);
    int next_row = sim->piece.row + drow;
    int next_col = sim->piece.col + dcol;
    if (!board_can_place(&sim->board, shape, sim->piece.rotation, next_row, next_col)) {
        return false;
    }

    sim->piece.row = next_row;
    sim->piece.col = next_col;
    if (drow != 0) {
        sim->lock_ticks = 0;
    }
    return true;
}

static bool sim_gravity_step(SimState *sim) {
    if (sim->locked) {
        return true;
    }

    if (sim_try_move(sim, 1, 0)) {
        return false;
    }

    ++sim->lock_ticks;
    if (sim->lock_ticks >= SIM_LOCK_THRESHOLD) {
        const PieceShape *shape = piece_shape_get((size_t)sim->piece.type);
        board_lock_shape(&sim->board, shape, sim->piece.rotation, sim->piece.row, sim->piece.col, sim->piece.type + 1);
        sim->locked = true;
        return true;
    }

    return false;
}

static void test_gravity_moves_piece_to_bottom(void) {
    SimState sim;
    sim_init(&sim, 0);

    int steps = 0;
    while (!sim_gravity_step(&sim)) {
        ++steps;
        assert(steps < BOARD_HEIGHT + 10);
    }

    assert(sim.locked);
    bool any_block = false;
    for (int col = 0; col < BOARD_WIDTH; ++col) {
        if (sim.board.cells[BOARD_HEIGHT - 1][col] != 0) {
            any_block = true;
            break;
        }
    }
    assert(any_block);
}

static void test_lock_delay_requires_multiple_ticks(void) {
    SimState sim;
    sim_init(&sim, 1);

    while (sim_try_move(&sim, 1, 0)) {
    }

    for (int i = 0; i < SIM_LOCK_THRESHOLD - 1; ++i) {
        assert(!sim_gravity_step(&sim));
        assert(!sim.locked);
    }
}

static void test_lateral_move_resets_lock_timer(void) {
    SimState sim;
    sim_init(&sim, 2);

    while (sim_try_move(&sim, 1, 0)) {
    }

    assert(sim.lock_ticks == 0);
    (void)sim_gravity_step(&sim);
    assert(sim.lock_ticks == 1);
    assert(sim_try_move(&sim, 0, -1));
    assert(sim.lock_ticks == 1);

    int ticks = 0;
    while (!sim.locked) {
        sim_gravity_step(&sim);
        ++ticks;
        assert(ticks < 10);
    }
}

static void test_hard_drop_matches_stepwise_fall(void) {
    SimState step_sim;
    sim_init(&step_sim, 3);

    SimState drop_sim = step_sim;

    int step_final_row = 0;
    while (!sim_gravity_step(&step_sim)) {
    }
    step_final_row = step_sim.piece.row;

    int drop_distance = 0;
    while (sim_try_move(&drop_sim, 1, 0)) {
        ++drop_distance;
    }
    const PieceShape *shape = piece_shape_get((size_t)drop_sim.piece.type);
    board_lock_shape(&drop_sim.board, shape, drop_sim.piece.rotation, drop_sim.piece.row, drop_sim.piece.col, drop_sim.piece.type + 1);
    drop_sim.locked = true;

    assert(drop_distance > 0);
    assert(step_final_row == drop_sim.piece.row);
}

int main(void) {
    run_test("gravity_moves_piece_to_bottom", test_gravity_moves_piece_to_bottom);
    run_test("lock_delay_requires_multiple_ticks", test_lock_delay_requires_multiple_ticks);
    run_test("lateral_move_resets_lock_timer", test_lateral_move_resets_lock_timer);
    run_test("hard_drop_matches_stepwise_fall", test_hard_drop_matches_stepwise_fall);
    return 0;
}
