#include "score.h"

#include <stdio.h>
#include <string.h>

// Score bookkeeping and persistence helpers.
static const int line_values[] = {0, 100, 300, 500, 800};

// Load the saved high score (when available) and prepare bookkeeping.
int score_state_init(ScoreState *state, const char *path) {
    if (state == NULL) {
        return -1;
    }

    state->current = 0;
    state->high = 0;
    const char *resolved = (path == NULL) ? SCORE_DEFAULT_FILE : path;
    strncpy(state->storage_path, resolved, SCORE_PATH_CAPACITY - 1);
    state->storage_path[SCORE_PATH_CAPACITY - 1] = '\0';

    FILE *fp = fopen(state->storage_path, "r");
    if (fp == NULL) {
        return 0;
    }

    int file_value = 0;
    if (fscanf(fp, "%d", &file_value) == 1 && file_value >= 0) {
        state->high = file_value;
    }

    fclose(fp);
    return 0;
}

// Persist the current high score to disk.
int score_state_save(const ScoreState *state) {
    if (state == NULL || state->storage_path[0] == '\0') {
        return -1;
    }

    FILE *fp = fopen(state->storage_path, "w");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "%d\n", state->high);
    fclose(fp);
    return 0;
}

// Clear the in-progress session score.
void score_reset_current(ScoreState *state) {
    if (state == NULL) {
        return;
    }
    state->current = 0;
}

// Award points for a batch of cleared lines (classic scoring table).
void score_add_lines(ScoreState *state, int cleared_lines) {
    if (state == NULL || cleared_lines <= 0) {
        return;
    }

    int award = 0;
    if (cleared_lines >= 0 && cleared_lines < (int)(sizeof(line_values) / sizeof(line_values[0]))) {
        award = line_values[cleared_lines];
    } else {
        award = line_values[4] + (cleared_lines - 4) * 100;
    }

    state->current += award;
}

// Increment score based on the distance of a hard drop.
void score_add_drop(ScoreState *state, int dropped_cells) {
    if (state == NULL || dropped_cells <= 0) {
        return;
    }

    state->current += dropped_cells * 2;
}

// Upgrade the saved high score if the current run beat it.
bool score_commit_highscore(ScoreState *state) {
    if (state == NULL) {
        return false;
    }

    if (state->current > state->high) {
        state->high = state->current;
        return true;
    }

    return false;
}
