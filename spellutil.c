#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "spellutil.h"

spell_list_node *
spell_list_init(void *data)
{
    spell_list_node *head = malloc(sizeof(spell_list_node));
    if (head == NULL) {
        return NULL;
    }

    head->next = NULL;
    head->data = data;
    return head;
}

int
spell_list_add(spell_list_node **phead, void *data)
{
    spell_list_node *tail;
    spell_list_node *head;
    spell_list_node *new_node;

    if (*phead == NULL) {
        return -1;
    }

    head = *phead;
    tail = head;

    while (tail->next) {
        tail = tail->next;
    }

    new_node = malloc(sizeof(spell_list_node));
    if (new_node == NULL) {
        return -1;
    }

    new_node->next = NULL;
    new_node->data = data;
    tail->next = new_node;
    return 0;
}

void
spell_list_free(spell_list_node **phead, void (*free_data) (void *))
{
    spell_list_node *n;
    spell_list_node *head;
    spell_list_node *next;

    if (!phead || !*phead) {
        return;
    }

    head = *phead;
    n = head;
    while (n) {
        next = n->next;
        if (free_data) {
            free_data(n->data);
        }
        free(n);
        n = next;
    }
    *phead = NULL;
}
