#include "main.h"


struct GameState create_blank_state()
{
    struct GameState new_state;
    for (int i = 0; i < WORD_LEN; i++) {
        new_state.disallowed_letters_mask[i] = MASK_ALLOWED;

        for (int j = 0; j < WORDS_IN_GAME; j++) {
            new_state.current_result_buf[j][i] = GRAY;

        }
    }

    return new_state;
}

struct GameState dupe_state(struct GameState *original_state)
{
    struct GameState copy = create_blank_state();

    for (int i = 0; i < WORD_LEN; i++) {
        copy.disallowed_letters_mask[i] = original_state->disallowed_letters_mask[i];

        for (int j = 0; j < WORD_LEN; j++) {
            copy.current_result_buf[j][i] = original_state->current_result_buf[j][i];
        }

        for (int j = 0; j < WORDS_IN_GAME; j++) {
            copy.answers[j][i] = original_state->answers[j][i];
        }
    }

    return copy;
}


void make_null(struct GameState *state, int i)
{  
    SET_TRUE_MASK(state->disallowed_letters_mask[STATE_NULL_LOC], STATE_NULL_BIT);  
    for (int j = 0; j < WORD_LEN; j++) {
        state->current_result_buf[i][j] = RESULT_NULL;
    }
}
bool is_null(struct GameState *state)
{  return CHECK_MASK(state->disallowed_letters_mask[STATE_NULL_LOC], STATE_NULL_BIT); }

void set_blank(struct GameState *state)
{  SET_TRUE_MASK(state->disallowed_letters_mask[STATE_BLANK_LOC], STATE_BLANK_BIT);  }
bool is_blank(struct GameState *state)
{  return CHECK_MASK(state->disallowed_letters_mask[STATE_BLANK_LOC], STATE_BLANK_BIT); }

bool is_marked_yellow(struct GameState *state, int col)
{  return CHECK_MASK(state->disallowed_letters_mask[STATE_YELLOW_LOC], (STATE_YELLOW_START_BIT+col) );  }
void mark_yellow(struct GameState *state, int col)
{  SET_TRUE_MASK(state->disallowed_letters_mask[STATE_YELLOW_LOC], (STATE_YELLOW_START_BIT+col));  }

bool is_marked_green(struct GameState *state, int col)
{  return CHECK_MASK(state->disallowed_letters_mask[STATE_GREEN_LOC], (STATE_GREEN_START_BIT+col));  }
void mark_green(struct GameState *state, int col)
{  SET_TRUE_MASK(state->disallowed_letters_mask[STATE_GREEN_LOC], (STATE_GREEN_START_BIT+col)); }

bool is_all_green(char *answer, char *solution)
{
    return memcmp(answer, solution, WORD_LEN) == 0;
}


void print_answer(struct GameState *state, char *answer, int i)
{

    for (int j = 0; j < WORD_LEN; j++) {
        if (state->current_result_buf[i][j] == GRAY) {
            printf(GRAY_TEXT);
        } else if (state->current_result_buf[i][j] == YELLOW) {
            printf(YELLOW_TEXT);
        } else if (state->current_result_buf[i][j] == GREEN) {
            printf(GREEN_TEXT);
        } else if (state->current_result_buf[i][j] == RESULT_NULL) {
            printf(RED_TEXT);
        }

        printf("%c"DEFAULT_TEXT, answer[j]);
    }
    printf(" ");
}


void get_results(char *solution, char *answer, struct GameState *state, int i)
{
    if (is_null(state))
        return;

    uint8_t taken_mask = 0;

    for (int j = 0; j < WORD_LEN; j++) {
        // check for disallowed letters in column
        if (CHECK_MASK(state->disallowed_letters_mask[j], CHAR_TO_INDEX(answer[j]))) {
            make_null(state, i);
            return;
        }

        if (is_marked_green(state, j)) {
            if (answer[j] != solution[j]) {
                make_null(state, i);
                return;
            }
            SET_TRUE_MASK(taken_mask, j);
        }

        else if (is_marked_yellow(state, j)) {
            bool contains_yellow = false;

            for (int k = 0; k < WORD_LEN; k++) {
                if (answer[k] == solution[j] && !CHECK_MASK(taken_mask, k)) {
                    SET_TRUE_MASK(taken_mask, k);
                    contains_yellow = true;
                    break;
                }
            }

            if (!contains_yellow) {
                make_null(state, i);
                return;
            }
        }        
    }

    // the characters in solution that are taken by another character
    taken_mask = 0;


    // update result buf

    for (int j = 0; j < WORD_LEN; j++)
        state->current_result_buf[i][j] = GRAY;

    for (int j = 0; j < WORD_LEN; j++) {

        // green
        if (answer[j] == solution[j]) {
            state->current_result_buf[i][j] = GREEN;
            mark_green(state, j);

            SET_TRUE_MASK(taken_mask, j);
            // set mask to only allow that character
            for (int k = 0; k < 26; k++) {
                if (k == CHAR_TO_INDEX(answer[j]))
                    continue;

                SET_TRUE_MASK(state->disallowed_letters_mask[j], k);
            }
        }
    }

    for (int j = 0; j < WORD_LEN; j++) {

        // yellow
        for (int k = 0; k < WORD_LEN; k++) {
            if (answer[j] == solution[k] && !CHECK_MASK(taken_mask, k)) { // solution contains letter
                state->current_result_buf[i][j] = YELLOW;
                mark_yellow(state, k);

                SET_TRUE_MASK(taken_mask, k);
                SET_TRUE_MASK(state->disallowed_letters_mask[j], CHAR_TO_INDEX(answer[j]));
                break;
            }
        }
    }

    for (int j = 0; j < WORD_LEN; j++) {
        if (state->current_result_buf[i][j] == GRAY) {
            for (int k = 0; k < WORD_LEN; k++) {
                SET_TRUE_MASK(state->disallowed_letters_mask[k], CHAR_TO_INDEX(answer[j]));
            }
        }
    }



}

