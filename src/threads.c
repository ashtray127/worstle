#include "main.h"

struct ThreadData {
    struct Words *possible_answers;
    int start;
    int end;
    struct GameState initial_state;
    char *solution;

    struct GameState *best_state;
    int *best_score;
    int *best_state_i;

    struct GameState *latest_state;
    int *latest_score;
    int *latest_state_i;

    int *states_ran;

    bool *finished;
};

void finish_line_threaded(struct GameState *state, int i, struct ThreadData *tdata)
{
    if (is_null(state)) 
        return;

    *(tdata->states_ran) = *(tdata->states_ran) + 1;

    int score = get_score(state, i);

    *(tdata->latest_state) = *state;
    *(tdata->latest_score) = score;
    *(tdata->latest_state_i) = i;

    if (score < *(tdata->best_score)) {
        *(tdata->best_score) = score;
        *(tdata->best_state) = *state;
        *(tdata->best_state_i) = i;
    }
}

void run_line_threaded(struct GameState state, int i, char *answer, char *solution, struct ThreadData *tdata)
{
    memcpy(state.answers[i], answer, WORD_LEN);

    get_results(solution, answer, &state, i);

    if (is_null(&state)) {
        return;
    }
    if (is_all_green(answer, solution)) {
        finish_line(&state, i+1, solution);
        return;
    }

    int score = get_score(&state, i);

    if (score > *(tdata->best_score)) {
        finish_line_threaded(&state, i, tdata);
        return;
    }

    if (i+1 >= WORDS_IN_GAME) {
        finish_line_threaded(&state, i+1, tdata);
        return;
    } 

    for (int j = 0; j < tdata->possible_answers->num_words; j++) {
        run_line_threaded(state, i+1, tdata->possible_answers->words[j], solution, tdata);
    }
}

void *run_thread(void *arg)
{
    struct ThreadData *tdata = (struct ThreadData*)arg;

    for (int i = tdata->start; i < tdata->end; i++) {
        run_line_threaded(tdata->initial_state, 0, tdata->possible_answers->words[i], tdata->solution, tdata);
    }

    return NULL;
}


void run_threads(struct Words *possible_answers, struct GameState initial_state, char *solution)
{
    float range_per = (float)(possible_answers->num_words-1) / ((float)NUM_THREADS);

    struct GameState best_states[NUM_THREADS];
    int best_scores[NUM_THREADS];
    int best_states_i[NUM_THREADS];
    struct GameState latest_states[NUM_THREADS];
    int latest_scores[NUM_THREADS];
    int latest_states_i[NUM_THREADS];
    bool threads_finished[NUM_THREADS];

    pthread_t threads[NUM_THREADS];

    struct ThreadData tdatas[NUM_THREADS];


    int states_ran = 0;

    for (int i = 0; i < NUM_THREADS; i++)  {
        best_states[i] = create_blank_state();
        latest_states[i] = create_blank_state();
        latest_states_i[i] = 0;
        best_states_i[i] = 0;
        
        best_scores[i] = INT32_MAX;
        latest_scores[i] = INT32_MAX;

        int start = range_per * i;
        int end = start + range_per;

        if (i == NUM_THREADS-1) { // account for rounding down by tacking on extras to last 
            end = possible_answers->num_words;
        }

        struct ThreadData data = { 
            possible_answers, 
            start, 
            end, 
            initial_state, 
            solution, 
            best_states+i,
            best_scores+i,
            best_states_i+i,
            latest_states+i,
            latest_scores+i,
            latest_states_i+i,

             &states_ran,
             threads_finished+i
        };

        tdatas[i] = data;


        pthread_create(threads+i, NULL, run_thread, tdatas + i);  
    }



    // while(1) {}

    // constantly print thread data
    while (1) {
        // clear the screen
        printf("\x1B""c");

        printf(
            "--- WORSTLE ---\n"
            "\n"
            "Solution: ");
        for (int j = 0; j < WORD_LEN; j++) 
            printf("%c", solution[j]);
        printf("\nStates Tried: %i", states_ran);
        printf("\n\n");
        printf("THREADS ---\n");

        bool all_threads_finished = true;
        for (int j = 0; j < NUM_THREADS; j++) {
            if (!threads_finished[j])
                all_threads_finished = false;

            printf("Thread %i:\n", j+1);
            printf("  Best Game: ");
            for (int k = 0; k < best_states_i[j]; k++) 
                print_answer(&best_states[j], best_states[j].answers[k], k);
            printf("\n  Best Score: %i", best_scores[j]);
            printf("\n\n  Latest Game: ");
            for (int k = 0; k < latest_states_i[j]; k++) 
                print_answer(&latest_states[j], latest_states[j].answers[k], k);
            printf("\n  Latest Score: %i\n\n", latest_scores[j]);
        }

        if (all_threads_finished) {
            break;
        }

        usleep(100 * 1000);
    }

}