#define _GNU_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "spellutil.h"

typedef struct int_struct {
    int i;
} int_struct;


static void
test_spell_hash_init()
{
    spell_hashtable *table = spell_hashtable_init(63);
    assert(table != NULL);
    assert(table->size == 64);
    assert(table->nfree == 64);
    printf("[PASSED] test_spell_hash\n");
    spell_hashtable_free(table, NULL);
}

static void
test_spell_hash_add_one()
{
    char *key1 = "hello world";
    char *val1 = "foo bar";
    spell_hashtable *table = spell_hashtable_init(1);
    spell_hashtable_add(table, key1, val1);
    char *val2 = spell_hashtable_get(table, key1);
    assert(strcmp(val1, val2) == 0);
    spell_hashtable_free(table, NULL);
    printf("[PASSED] test_spell_hash_add_one\n");

}

static void
test_spell_hash_add_multi()
{
    char *key1 = "hello world";
    char *val1 = "foo bar";
    char *key2 = "a";
    char *val2 = "b";
    char *key3 = "b ";
    char *val3 = "d";
    spell_hashtable *table = spell_hashtable_init(1);
    spell_hashtable_add(table, key1, val1);
    spell_hashtable_add(table, key2, val2);
    spell_hashtable_add(table, key3, val3);
    assert(strcmp(val1, spell_hashtable_get(table, key1)) == 0);
    assert(strcmp(val2, spell_hashtable_get(table, key2)) == 0);
    assert(strcmp(val3, spell_hashtable_get(table, key3)) == 0);
    spell_hashtable_free(table, NULL);
    printf("[PASSED] test_spell_hash_add_multi\n");

}

static void
test_spell_hash_get()
{
    char *key1 = "hello world";
    char *val1 = "foo bar";
    char *key2 = "a";
    char *val2 = "b";
    char *key3 = "c";
    char *val3 = "d";
    spell_hashtable *table = spell_hashtable_init(1);
    spell_hashtable_add(table, key1, val1);
    spell_hashtable_add(table, key2, val2);
    spell_hashtable_add(table, key3, val3);
    char *getval1 = spell_hashtable_get(table, key1);
    char *getval2 = spell_hashtable_get(table, key2);
    char *getval3 = spell_hashtable_get(table, key3);
    assert(strcmp(getval1, val1) == 0);
    assert(strcmp(getval2, val2) == 0);
    assert(strcmp(getval3, val3) == 0);
    spell_hashtable_free(table, NULL);
    printf("[PASSED] test_spell_hash_get\n");

}


static void
test_spell_hash_update()
{
    char *key1 = "hello world";
    int val1 = 1;
    int val2 = 2;

    spell_hashtable *table = spell_hashtable_init(32);
    spell_hashtable_add(table, key1, &val1);
    assert(*(int *) spell_hashtable_get(table, key1) == val1);
    spell_hashtable_add(table, key1, &val2);
    assert(*(int *) spell_hashtable_get(table, key1) == val2);
    spell_hashtable_free(table, NULL);
    printf("[PASSED] test_spell_hash_update\n");

}

static int
count_digits(int x, int ndigits)
{
    int i;
    if (x < 10) {
        return ndigits + 1;
    }

    return count_digits(x / 10, ndigits + 1);
}

static char *
itos(int x)
{
    int ndigits = count_digits(x, 0);
    char *s = malloc(ndigits + 1);
    int i;
    int n = x;
    s[ndigits] = 0;
    for (i = ndigits - 1; i >= 0; i--) {
        s[i] = n % 10 + 48;
        n /= 10;
    }
    return s;
}

static void
test_spell_hash_resize()
{
    int i;
    spell_hashtable *table = spell_hashtable_init(1);
    for (i = 0; i < table->size; i++) {
        char *k = itos(i);
        spell_hashtable_add(table, k, &i);
        free(k);
    }
    assert(table->size == SPELL_HASHTABLE_INIT_SIZE);
    assert(table->nfree == 0);
    spell_hashtable_add(table, "33", "33");
    assert(table->size == 2 * SPELL_HASHTABLE_INIT_SIZE);
    spell_hashtable_free(table, NULL);
    printf("[PASSED] test_spell_hash_resize\n");

}

static spell_hashtable *
generate_hashtable(int limit)
{
    int i;
    int_struct *s;
    spell_hashtable *table = spell_hashtable_init(limit);
    for (i = 0; i < limit; i++) {
        char *k = itos(i);
        s = malloc(sizeof(int_struct));
        s->i = i;
        spell_hashtable_add(table, k, (void *)s);
        free(k);
    }
    return table;
}

static char *
print_struct_int(void *data)
{
    char *s;
    int_struct *d  = (int_struct *) data;
    asprintf(&s, "%d", d->i);
    return s;
}

static void
test_spell_hash_dump()
{
    const int SIZE = 65;
    spell_hashtable *table = generate_hashtable(SIZE);
    spell_hashtable_dump(table, "table.dump", print_struct_int);
    spell_hashtable_free(table, free);
}

static void
test_spell_hash_remove()
{
    spell_hashtable *table = generate_hashtable(1);
    srand(time(NULL));
    int r;
    char *k;
//    r = (rand() % 25) + 1;
  //  k = itos(r);
    //assert(k != NULL);
    //assert(table != NULL);
    spell_hashtable_remove(table, "0", free);
    assert(spell_hashtable_get(table, "0") == NULL);
    //free(k);
    spell_hashtable_free(table, free);
    printf("[PASSED] test_spell_hash_remove\n");
}


int
main(int argc, char **argv)
{
    test_spell_hash_init();
    test_spell_hash_add_one();
    test_spell_hash_add_multi();
    test_spell_hash_get();
    test_spell_hash_resize();
    test_spell_hash_update();
    test_spell_hash_dump();
    test_spell_hash_remove();
}
