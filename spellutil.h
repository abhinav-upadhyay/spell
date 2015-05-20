#ifndef SPELLUTIL_H
#define SPELLUTIL_H

#include <sys/types.h>
#include <stdbool.h>

#define SPELL_HASHTABLE_INIT_SIZE 32

typedef struct spell_list_node {
    struct spell_list_node *next;
    void *data;
} spell_list_node;


typedef struct spell_hashtable_t {
    spell_list_node **array;
    size_t size;
    size_t nfree;
} spell_hashtable;

typedef struct keyval {
    char *key;
    void *val;
} keyval;

spell_list_node *spell_list_init(void *);
int spell_list_add_head(spell_list_node **, void *);
void spell_list_remove(spell_list_node **, spell_list_node *, void (*) (void *));
void spell_list_free(spell_list_node **, void (*) (void *));
spell_list_node *spell_list_get_tail(spell_list_node *);
spell_list_node *spell_list_get(spell_list_node *, void *, int (*) (const void *, const void*));
spell_hashtable *spell_hashtable_init(size_t);
void spell_hashtable_add(spell_hashtable *, char *, void *);
void spell_hashtable_remove(spell_hashtable *, char *, void (*)(void *));
void *spell_hashtable_get(spell_hashtable *, char *);
void spell_hashtable_free(spell_hashtable *, void (*) (void *));
void spell_hashtable_dump(spell_hashtable *, const char *, char *(*print) (void *));
spell_list_node * spell_hashtable_get_key_values(spell_hashtable *);
#endif
