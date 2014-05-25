#ifndef SPELLUTIL_H
#define SPELLUTIL_H

typedef struct spell_list_node {
    struct spell_list_node *next;
    void *data;
} spell_list_node;

typedef struct spell_hashtable {
    char *key;
    spell_list_node *val;
} spell_hashtable;

spell_list_node *spell_list_init(void *);
int spell_list_add(spell_list_node **, void *);
void spell_list_remove(spell_list_node **, spell_list_node *);
void spell_list_free(spell_list_node **, void (*) (void *));
spell_hashtable *spell_hashtable_init(size_t);
void spell_hashtable_add(spell_hashtable *, char *, void *);
void spell_hashtable_remove(spell_hashtable *, char *);
void spell_hashtable_get(spell_hashtable *, char *);
void spell_hashtable_free(spell_hashtable *, void (*) (void *));

#endif
