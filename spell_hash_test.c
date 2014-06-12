#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spellutil.h"

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
    char *key3 = "c";
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
        s[i] = n % 10;
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
    assert(table->size == 32);
    assert(table->nfree == 0);
    spell_hashtable_add(table, "33", "33");
    assert(table->size == 64);
    assert(table->nfree == 31);
    spell_hashtable_free(table, NULL);
    printf("[PASSED] test_spell_hash_resize\n");

}

int
main(int argc, char **argv)
{
    test_spell_hash_init();
    test_spell_hash_add_one();
    test_spell_hash_add_multi();
    test_spell_hash_resize();
}
