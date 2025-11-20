#include "piece.h"

// Static definitions for every tetromino shape and rotation.
static const PieceShape g_piece_defs[] = {
    {4, 2, {"0000111100000000", "0010001000100010", NULL, NULL}},
    {4, 1, {"0011001100000000", NULL, NULL, NULL}},
    {4, 4, {"0000010011100000", "0010011000100000", "0000111001000000", "0100011001000000"}},
    {4, 4, {"0010111000000000", "0100010001100000", "0000111010000000", "1100010001000000"}},
    {4, 4, {"1000111000000000", "0110010001000000", "0000111000100000", "0100010011000000"}},
    {4, 2, {"0110110000000000", "0100011000100000", NULL, NULL}},
    {4, 2, {"1100011000000000", "0010011001000000", NULL, NULL}}
};

static const size_t PIECE_COUNT = sizeof(g_piece_defs) / sizeof(g_piece_defs[0]);

// Return how many unique shapes are available.
size_t piece_shape_count(void) {
    return PIECE_COUNT;
}

// Get a shape descriptor by index, or NULL if out-of-range.
const PieceShape *piece_shape_get(size_t index) {
    if (index >= PIECE_COUNT) {
        return NULL;
    }
    return &g_piece_defs[index];
}

// Utility used by tests/board logic to examine individual cells.
bool piece_shape_cell_filled(const PieceShape *shape, int rotation, int local_row, int local_col) {
    if (shape == NULL || rotation < 0 || rotation >= shape->rotation_count) {
        return false;
    }

    if (local_row < 0 || local_row >= shape->size || local_col < 0 || local_col >= shape->size) {
        return false;
    }

    const char *pattern = shape->rotations[rotation];
    return pattern[local_row * shape->size + local_col] == '1';
}
