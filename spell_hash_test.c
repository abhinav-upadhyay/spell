#include <assert.h>
#include <stdio.h>
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
    char *key2 = spell_hashtable_get(table, key1);
    assert(strcmp(key1, key2) == 0);
    spell_hashtable_free(table, NULL);
    printf("[PASSED] test_spell_hash_add_one\n");

}

int
main(int argc, char **argv)
{
    test_spell_hash_init();
    test_spell_hash_add_one();
}
