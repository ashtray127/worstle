#include "main.h"


static struct BestState best;


void set_best()
{
    best.best_score = INT32_MAX;
}

int get_score(struct GameState *state, int i)
{
    int current_score = 0;
    for (int game = 0; game < i; game++) {
        for (int col = 0; col < WORD_LEN; col++) {
            switch (state->current_result_buf[game][col]) {
            case GRAY:
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

    return current_score;
}

void finish_line(struct GameState *state, int i, char *solution)
{
    static int states_finished = 0;
    if (i == 0)
        return;

    states_finished++;
    if (is_null(state)) {
        return;
    }
    int current_score = get_score(state, i);
    if (current_score < best.best_score) {

        best.best_score = current_score;
        best.best_state = (*state);
        best.best_state_i = i;
    }

    if (states_finished % 1000 != 0) {
        return;
    }

    printf("\x1B""c");
    printf("States Tried: %i\n""Best State:\n""  Words: ", states_finished);
    for (int j = 0; j < best.best_state_i; j++) 
        print_answer(&best.best_state, best.best_state.answers[j], j);
    printf("\n  Score: %i\n\nLast State:\n  Words: ", best.best_score);
    for (int j = 0; j < i; j++) 
        print_answer(state, state->answers[j], j);
    printf("\n  Score: %i\n", current_score);
    

}

void run_line(struct Words *possible_words, struct GameState state, char *solution, int i, char *answer)
{
    memcpy(state.answers[i], answer, WORD_LEN);

    get_results(solution, state.answers[i], &state, i);



    if (is_null(&state)) {
        return;
    }
    if (is_all_green(answer, solution)) {
        finish_line(&state, i+1, solution);
        return;
    }

    int score = get_score(&state, i);

    if (score >= best.best_score) {
        finish_line(&state, i, solution);
        return;
    }

    if (i+1 >= WORDS_IN_GAME) {
        finish_line(&state, i+1, solution);
        return;
    } 

    for (int j = 0; j < possible_words->num_words; j++) {
        run_line(possible_words, state, solution, i+1, possible_words->words[j]);
    }

}
