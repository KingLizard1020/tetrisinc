#include "bag.h"

#include <stdlib.h>

// Implements the seven-bag style randomizer used for piece order.

// Refill and shuffle the bag using Fisher-Yates so every piece appears once.
static void piece_bag_refill(PieceBag *bag) {
    if (bag == NULL || bag->piece_count == 0) {
        return;
    }

    for (size_t i = 0; i < bag->piece_count; ++i) {
        bag->values[i] = (int)i;
    }

    for (size_t i = bag->piece_count; i > 1; --i) {
        size_t j = (size_t)(rand() % (int)i);
        size_t idx = i - 1;
        int tmp = bag->values[idx];
        bag->values[idx] = bag->values[j];
        bag->values[j] = tmp;
    }

    bag->cursor = 0;
}

// Prepare a bag with the provided number of unique pieces.
void piece_bag_init(PieceBag *bag, size_t piece_count) {
    if (bag == NULL) {
        return;
    }

    if (piece_count > PIECE_BAG_MAX) {
        piece_count = PIECE_BAG_MAX;
    }

    bag->piece_count = piece_count;
    bag->cursor = piece_count;

    if (piece_count > 0) {
        piece_bag_refill(bag);
    }
}

// Retrieve the next piece id, refilling automatically when exhausted.
int piece_bag_next(PieceBag *bag) {
    if (bag == NULL || bag->piece_count == 0) {
        return -1;
    }

    if (bag->cursor >= bag->piece_count) {
        piece_bag_refill(bag);
    }

    return bag->values[bag->cursor++];
}
