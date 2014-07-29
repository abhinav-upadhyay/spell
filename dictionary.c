#define _GNU_SOURCE
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spellutil.h"

typedef struct word_count {
    int value;
} count;

static void
usage(void)
{
    fprintf(stderr, "Usage: spell -i InputFile -o outputFile\n");
    exit(1);
}

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
    if (s == NULL) {
        return;
    }
    for (int i = 0; i < strlen(s); i++) {
        c = s[i];
        s[i] = tolower(c);
    }
}

static int
is_upper(char *s)
{
    while (*s) {
        if (isupper(*s++)) {
            continue;
        }
        /* not uppercase string */
        return 0;
    }
    /* uppercase string */
    return 1;
}


static char *
sanitize_string(char *s)
{
    size_t len = strlen(s);
    char *ret = malloc(len + 1);
    int i = 0;
    if (s[0] == '(' && s[len - 1] == ')') {
        s[len - 1] = 0;
        if (is_upper(s + 1)) {
            return NULL;
        }
    }

    if (is_upper(s)) {
        return NULL;
    }

    while(*s) {
        /*
         * Detect apostrophe and stop copying characters immediately
         */
        if ((*s == '\'') && (
                 !strncmp(s + 1, "s", 1) ||
                 !strncmp(s + 1, "es", 2) ||
                 !strncmp(s + 1, "m", 1) ||
                 !strncmp(s + 1, "d", 1) ||
                 !strncmp(s + 1, "ll", 2))) {
            break;
        }

        /*
         * If the word contains a dot in between that suggests it is either
         * an abbreviation or somekind of a URL. Do not bother with such words.
         */
        if (*s == '.') {
            free(ret);
            return NULL;
        }

        if (!isalpha(*s)) {
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
            wordlen = strcspn(templine, " ,-:\u2014");
            templine[wordlen] = 0;
            word = templine;
            templine += wordlen + 1;
            char *sanitized_word = sanitize_string(word);
            if (sanitized_word == NULL) {
                continue;
            }
            if (strlen(sanitized_word) == 0) {
                free(sanitized_word);
                continue;
            }
            to_lower(sanitized_word);
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
    char ch;
    char *input_file = NULL;
    char *output_file = NULL;

    while ((ch = getopt(argc, argv, "i:o:")) != -1) {
        switch (ch) {
        case 'i':
            input_file = optarg;
            break;
        case 'o':
            output_file = optarg;
            break;
        case '?':
        default:
            usage();
        }
    }

    if (!input_file || !output_file) {
        usage();
    }

    spell_hashtable *table = spell_hashtable_init(1024);
    if (table == NULL) {
        errx(EXIT_FAILURE, "Failed to initialize the hash table");
    }
    parse_file(input_file, table);
    spell_hashtable_dump(table, output_file, print_count);
    spell_hashtable_free(table, free_count);
}
