#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define POSSIBLE_ANSWERS_PATH "./data/answers.txt"
#define POSSIBLE_SOLUTIONS_PATH "./data/solutions.txt"

#define WORD_LEN 5
#define WORDS_IN_GAME 6

#define CHECK_MASK(mask, digit) ((mask >> digit) % 0b10)
#define SET_TRUE_MASK(mask, digit) (mask |= (0b1 << digit))

#define C

/** Converts a character to it's index in the alphabet (a -> 0, b -> 1, etc.) */
#define CHAR_TO_INDEX(c) (( (int)c ) - 97)
#define INDEX_TO_CHAR(c) ( (char)(c+97) )

#define GRAY 0
#define YELLOW 1
#define GREEN 2
#define RESULT_NULL 3

#define RES_NULL 1
#define RES_NOT_NULL 0

#define MASK_ALLOWED 0
#define MASK_DISALLOWED 1

#define STATE_NULL_BIT 31
#define STATE_NULL_LOC 0
#define STATE_BLANK_BIT 31
#define STATE_BLANK_LOC 1
#define STATE_GREEN_LOC 0
#define STATE_GREEN_START_BIT 26
#define STATE_YELLOW_LOC 1
#define STATE_YELLOW_START_BIT 26

#define GREEN_TEXT "\x1B[32m"
#define YELLOW_TEXT "\x1B[33m"
#define GRAY_TEXT "\x1B[2m"
#define RED_TEXT "\x1B[31m"
#define DEFAULT_TEXT "\x1B[39m\x1B[22m"



struct Words {
    char **words;
    int num_words;
};

struct GameState {
    uint32_t disallowed_letters_mask[WORD_LEN];

    uint8_t current_result_buf[WORDS_IN_GAME][WORD_LEN];
    char answers[WORDS_IN_GAME][WORD_LEN];
};

struct GameState create_blank_state();

// files.c -------------------
struct Words get_words_from_file(char path[]);
void cleanup_word_file(struct Words *words);

// words.c ----------------------
struct GameState create_blank_state();
struct GameState dupe_state(struct GameState *original_state);
void make_null(struct GameState *state, int i);
bool is_null(struct GameState *state);
void set_blank(struct GameState *state);
bool is_blank(struct GameState *state);
bool is_marked_yellow(struct GameState *state, int col);
void mark_yellow(struct GameState *state, int col);
bool is_marked_green(struct GameState *state, int col);
void mark_green(struct GameState *state, int col);
bool is_all_green(struct GameState *state, int i);
void print_answer(struct GameState *state, char *answer, int i);
void get_results(char *solution, char *answer, struct GameState *state, int i);

// game.c -----------------------
void finish_line(struct GameState *state, int i, char *solution);
void run_line(struct Words *possible_words, struct GameState state, char *solution, int i, char *answer);
