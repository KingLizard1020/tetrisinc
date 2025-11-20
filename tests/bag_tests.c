#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "bag.h"

static void run_test(const char *name, void (*fn)(void)) {
    printf("[RUN] %s\n", name);
    fn();
    printf("[OK ] %s\n", name);
}

static void test_bag_cycle_contains_all(void) {
    PieceBag bag;
    piece_bag_init(&bag, 7);

    int seen[7];
    memset(seen, 0, sizeof(seen));

    for (int i = 0; i < 7; ++i) {
        int value = piece_bag_next(&bag);
        assert(value >= 0 && value < 7);
        ++seen[value];
    }

    for (int i = 0; i < 7; ++i) {
        assert(seen[i] == 1);
    }
}

static void test_bag_multiple_cycles(void) {
    PieceBag bag;
    piece_bag_init(&bag, 7);

    int counts[7];
    memset(counts, 0, sizeof(counts));

    for (int i = 0; i < 14; ++i) {
        int value = piece_bag_next(&bag);
        assert(value >= 0 && value < 7);
        ++counts[value];
    }

    for (int i = 0; i < 7; ++i) {
        assert(counts[i] == 2);
    }
}

int main(void) {
    run_test("bag_cycle_contains_all", test_bag_cycle_contains_all);
    run_test("bag_multiple_cycles", test_bag_multiple_cycles);
    return 0;
}
