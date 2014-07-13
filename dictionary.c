#define _GNU_SOURCE
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spellutil.h"

typedef struct word_count {
    int value;
} count;

static char *
print_count(void *data)
{
    count *wc = (count *) data;
    char *s;
    asprintf(&s, "%d", wc->value);
    return s;
}

static void
free_count(void *data)
{
    count *c = (count *) data;
    free(c);
}

int
parse_file(const char *filename, spell_hashtable *table)
{
    FILE *file = fopen(filename, "r");
    int _errno;
    size_t len = 0;
    ssize_t read;
    char *word = NULL;
    count *c;
    count *default_count;

    if (file == NULL) {
        _errno = errno;
        fprintf(stderr, "fopen failed for file %s", filename);
        return errno;
    }

    while ((read = getdelim(&word, &len, ' ', file)) != -1) {
        // TODO Use a more sophisitcated tokenization,
        // perhaps preprocess with a stemmer algorithm first
        word[read - 1] = 0;
        c = (count *) spell_hashtable_get(table, word);
        if (c == NULL) {
            default_count = malloc(sizeof(count));
            default_count->value = 1;
            spell_hashtable_add(table, word, default_count);
        } else {
            c->value++;
            spell_hashtable_add(table, word, c);
        }
    }
    free(word);
    fclose(file);
    return 0;
}

int
main(int argc, char **argv)
{
    spell_hashtable *table = spell_hashtable_init(10);
    if (table == NULL) {
        errx(EXIT_FAILURE, "Failed to initialize the hash table");
    }
    parse_file("file.txt", table);
    spell_hashtable_dump(table, "dictionary.dump", print_count);
    spell_hashtable_free(table, free_count);

}

