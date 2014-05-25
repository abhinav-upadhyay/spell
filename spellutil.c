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

void
spell_list_remove(spell_list_node **phead, spell_list_node *node, void (*pfree) (void *))
{
    if (!phead || !node) {
        return;
    }

    spell_list_node *head = *phead;
    spell_list_node *iter = head;
    spell_list_node *temp_node;

    /*
     * if the node to be deleted is the head of the linked list
     */
    if (node == head) {
        *phead = head->next;
        if (pfree) {
            pfree(head->data);
        }
        free(head);
        return;
    }

    while (iter) {
        if (iter->next == node) {
            iter->next = iter->next->next;
            if (pfree) {
                pfree(node->data);
            }
            free(node);
            return;
        }
        iter = iter->next;
    }
}

spell_list_node *
spell_list_get_tail(spell_list_node *head)
{
    spell_list_node *i = head;
    if (i == NULL) {
        return NULL;
    }

    while (i->next) {
        i = i->next;
    }
    return i;
}
