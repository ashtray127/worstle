#include "main.h"


void finish_line(struct GameState *state, int i, char *solution)
{
    static int states_finished = 0;
    static int best_finish = INT32_MAX;
    static int best_state_i = 0;
    static struct GameState best_state;

    states_finished++;
    if (is_null(state)) {
        return;
    }

    int current_score = 0;
    for (int game = 0; game < i; game++) {
        for (int col = 0; col < WORD_LEN; col++) {
            switch (state->current_result_buf[game][col]) {
            case GRAY:
                current_score--;
            break;
            case YELLOW:
                current_score++;
            break;
            case GREEN:
                current_score += 2;
            break;
            }
        }
    }
    if (current_score < best_finish) {
        best_finish = current_score;
        best_state = (*state);
        best_state_i = i;
    }

    if (states_finished % 10 != 0) {
        return;
    }

    printf("\x1B""c");
    printf("States Tried: %i\n""Best State:\n""  Words: ", states_finished);
    for (int j = 0; j < best_state_i; j++) 
        print_answer(&best_state, best_state.answers[j], j);
    printf("\n  Score: %i\n\nLast State:\n  Words: ", best_finish);
    for (int j = 0; j < i; j++) 
        print_answer(state, state->answers[j], j);
    printf("\n  Score: %i\n", current_score);
    

}

void run_line(struct Words *possible_words, struct GameState state, char *solution, int i, char *answer)
{
    memcpy(state.answers[i], answer, WORD_LEN);

    get_results(solution, state.answers[i], &state, i);

    if (is_null(&state)) {
        finish_line(&state, i+1, solution);
        return;
    }
    if (is_all_green(&state, i)) {
        finish_line(&state, i+1, solution);
        return;
    }

    if (i+1 >= WORDS_IN_GAME) {
        finish_line(&state, i, solution);
        return;
    } 

    for (int j = 0; j < possible_words->num_words; j++) {
        run_line(possible_words, state, solution, i+1, possible_words->words[j]);
    }
}

