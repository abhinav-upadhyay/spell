#include <assert.h>
#include <stdbool.h>
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
    assert(table->nfree == SPELL_HASHTABLE_INIT_SIZE - 1);
    spell_hashtable_free(table, NULL);
    printf("[PASSED] test_spell_hash_resize\n");

}

static spell_hashtable *
generate_hashtable(int limit)
{
    int i;
    spell_hashtable *table = spell_hashtable_init(limit);
    for (i = 0; i < limit; i++) {
        char *k = itos(i);
        spell_hashtable_add(table, k, &i);
        free(k);
    }
    return table;
}

static void
test_spell_hash_get_keys()
{
    int i;
    const int SIZE = 65;
    int arr[65] = {0};
    spell_hashtable *table = generate_hashtable(SIZE);
    spell_list_node *keylist = spell_hashtable_get_keys(table, false);
    assert(keylist != NULL);
    spell_list_node *thead = keylist;
    while (thead) {
        char *key = (char *) thead->data;
        int intkey = atoi(key);
        assert(arr[intkey] == 0);
        arr[intkey] = intkey;
        thead = thead->next;
    }
    for (i = 0; i < SIZE; i++) {
        assert(arr[i] == i);
    }
    spell_hashtable_free(table, NULL);
    spell_list_free(&keylist, NULL);
    printf("[PASSED] test_spell_hash_get_keys\n");
}



int
main(int argc, char **argv)
{
    test_spell_hash_init();
    test_spell_hash_add_one();
    test_spell_hash_add_multi();
    test_spell_hash_resize();
    test_spell_hash_get_keys();
}
