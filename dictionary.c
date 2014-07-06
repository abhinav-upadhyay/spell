#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spellutil.h"
#include "dictionary.h"

typedef struct word_count {
    int value;
} count;

int
parse_file(const char *filename, spell_hashtable *table)
{
    FILE *file = fopen(filename, "r");
    int _errno;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *word;
    count *c;
    count default_count;
    default_count.value = 1;

    if (file == NULL) {
        _errno = errno;
        fprintf(stderr, "fopen failed for file %s", filename);
        return errno;
    }

    while ((read = getline(&line, &len, file)) != -1) {
        while ((word = strchr(line, ' ')) != NULL) {
            c = (count *) spell_hashtable_get(table, word);
            if (c == NULL) {
                spell_hashtable_add(table, word, &default_count);
            } else {
                c->value++;
                spell_hashtable_add(table, word, c);
            }
            free(line);
        }
    }
    fclose(file);
    return 0;
}
