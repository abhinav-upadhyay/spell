#define _GNU_SOURCE
#include <ctype.h>
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
    free(data);
}

static void
to_lower(char *s)
{
    char c;
    char *t = s;
    for (int i = 0; i < strlen(s); i++) {
        c = s[i];
        s[i] = tolower(c);
    }
}

static char *
sanitize_string(char *s)
{
    char *ret = malloc(strlen(s) + 1);
    int i = 0;
    while(*s) {
        if (*s == ' ' || *s == '\t' || *s == '"' || *s == '\'' ||
                *s == ',' || *s == '.' || *s == ';' ||
                *s == ':') {
            s++;
            continue;
        }
        ret[i++] = *s++;
    }
    ret[i] = 0;
    return ret;
}

static int
is_valid(char *word)
{
    if (!strcmp(word, " ") ||
            !strcmp(word, "\n") ||
            !strcmp(word, "\t")) {
        return 1;
    }
    return 0;
}

int
parse_file(const char *filename, spell_hashtable *table)
{
    FILE *file = fopen(filename, "r");
    int _errno;
    size_t len = 0;
    size_t wordlen;
    ssize_t read;
    char *word = NULL;
    char *line = NULL;
    char *templine;
    count *c;
    count *default_count;

    if (file == NULL) {
        _errno = errno;
        fprintf(stderr, "fopen failed for file %s", filename);
        return errno;
    }

    while ((read = getline(&line, &len, file)) != -1) {
        // TODO Use a more sophisitcated tokenization,
        // perhaps preprocess with a stemmer algorithm first
        line[read - 1] = 0;
        templine = line;
        while (*templine) {
            wordlen = strcspn(templine, " ,-:");
            templine[wordlen] = 0;
            word = templine;
            templine += wordlen + 1;
            to_lower(word);
            char *sanitized_word = sanitize_string(word);
            if (sanitized_word == NULL || strlen(sanitized_word) == 0) {
                free(sanitized_word);
                continue;
            }
            c = (count *) spell_hashtable_get(table, sanitized_word);
            if (c == NULL) {
                default_count = malloc(sizeof(count));
                default_count->value = 1;
                spell_hashtable_add(table, sanitized_word, default_count);
            } else {
                c->value++;
            }
            free(sanitized_word);
        }
    }
    free(line);
    fclose(file);
    return 0;
}

int
main(int argc, char **argv)
{
    spell_hashtable *table = spell_hashtable_init(1024);
    if (table == NULL) {
        errx(EXIT_FAILURE, "Failed to initialize the hash table");
    }
    parse_file("file.txt", table);
    spell_hashtable_dump(table, "dictionary.dump", print_count);
    spell_hashtable_free(table, free_count);

}

