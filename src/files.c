#include "main.h"

struct Words get_words_from_file(char path[])
{
    FILE *fptr = fopen(path, "r");

    size_t num_of_words_size = 100;
    unsigned int num_of_words = 0;
    char **words = malloc(num_of_words_size * sizeof(char*));

    while (1) {
        char *new_word = malloc(6 * sizeof(char));
        if (!fgets(new_word, 6, fptr)) 
            break;
        
        fseek(fptr, 1, SEEK_CUR); // skip new line characters

        words[num_of_words] = new_word;
        num_of_words++;

        if (num_of_words >= num_of_words_size) {
            num_of_words_size += 100;
            words = realloc(words, num_of_words_size * sizeof(char*));
        }
    }

    fclose(fptr);

    // resize words to trim extra space
    words = realloc(words, num_of_words * sizeof(char*));

    struct Words result = { words, num_of_words };

    return result;
}

void cleanup_word_file(struct Words *words) 
{
    for (int i = 0; i < words->num_words; i++) {
        free(words->words[i]);
    }
    free(words->words);
}