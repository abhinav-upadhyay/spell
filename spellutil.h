#ifndef LIBSPELL_H
#define LIBSPELL_H

typedef struct spell_list_node {
    struct spell_list_node *next;
    void *data;
} spell_list_node;

spell_list_node *spell_list_init(void *);
int spell_list_add(spell_list_node **, void *);
void spell_list_free(spell_list_node **, void (*) (void *));

#endif
