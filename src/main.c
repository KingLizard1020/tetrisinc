#include <stdio.h>
#include <stdlib.h>

#include "game.h"

int main(void) {
    if (game_init() != 0) {
        fprintf(stderr, "Failed to initialize game.\n");
        return EXIT_FAILURE;
    }

    game_loop();
    game_shutdown();

    return EXIT_SUCCESS;
}
