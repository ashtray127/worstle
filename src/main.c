#include "main.h"


int main()
{
    set_best();
    struct Words possible_answers = get_words_from_file(POSSIBLE_ANSWERS_PATH);
    // struct Words possible_answers = get_words_from_file("./data/test.txt");
    char *solution = "gooey";

    struct GameState initial_state = create_blank_state();

    // for (int j = 0; j < possible_answers.num_words; j++) {
    //     run_line(&possible_answers, initial_state, solution, 0, possible_answers.words[j]);
    // }

    run_threads(&possible_answers, initial_state, solution);


    cleanup_word_file(&possible_answers);
    return 0;
}