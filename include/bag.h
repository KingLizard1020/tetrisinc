#ifndef BAG_H
#define BAG_H

#include <stddef.h>

#define PIECE_BAG_MAX 16

typedef struct {
    int values[PIECE_BAG_MAX];
    size_t piece_count;
    size_t cursor;
} PieceBag;

void piece_bag_init(PieceBag *bag, size_t piece_count);
int piece_bag_next(PieceBag *bag);

#endif /* BAG_H */
