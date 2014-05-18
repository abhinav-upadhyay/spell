#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "spellutil.h"

static void
test_list_init()
{
    int i = 10;
    spell_list_node *n = spell_list_init(&i);
    assert(n != NULL);
    assert(n->next == NULL);
    assert(*(int *)n->data == i);
    printf("[PASSED] test_list_init\n");
    spell_list_free(&n, NULL);
}

static void
test_list_add_one()
{
    int i = 10;
    spell_list_node *n = spell_list_init(&i);
    assert(n != NULL);
    assert(n->next == NULL);
    int status = spell_list_add(&n, &i);
    assert(n->next != NULL);
    assert(*(int *)n->next->data == i);
    assert(n->next->next == NULL);
    printf("[PASSED] test_list_add_one\n");
    spell_list_free(&n, NULL);
}

static void
test_list_add_multi()
{
    int i = 0;
    int j = 1;
    int k = 2;
    int l = 3;

    spell_list_node *head = spell_list_init(&i);
    spell_list_add(&head, &j);
    spell_list_add(&head, &k);
    spell_list_add(&head, &l);
    assert(*(int *) head->data == i);
    assert(* (int *) head->next->data == j);
    assert(* (int *) head->next->next->data == k);
    assert(* (int *) head->next->next->next->data == l);
    spell_list_free(&head, NULL);
    printf("[SUCCESS] test_list_add_multi\n");
}

static void
test_list_free_one()
{
    int i = 10;
    spell_list_node *n = spell_list_init(&i);
    assert(n != NULL);
    spell_list_free(&n, NULL);
    assert(n == NULL);
    printf("[SUCCESS] test_list_free_one\n");
}


int main
(int argc, char **argv)
{
    test_list_init();
    test_list_add_one();
    test_list_add_multi();
    test_list_free_one();
}
