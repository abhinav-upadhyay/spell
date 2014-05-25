#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "spellutil.h"

static spell_list_node *
setup()
{
    int i = 10;
    spell_list_node *n = spell_list_init(&i);
    assert(n != NULL);
    assert(n->next == NULL);
    assert(*(int *)n->data == i);
    return n;
}

static void
test_list_init()
{
    spell_list_node *n = setup();
    printf("[PASSED] test_list_init\n");
    spell_list_free(&n, NULL);
}

static void
test_list_add_one()
{
    int j = 20;
    spell_list_node *n = setup();
    int status = spell_list_add(&n, &j);
    assert(n->next != NULL);
    assert(*(int *)n->next->data == j);
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
    printf("[PASSED] test_list_add_multi\n");
}

static void
test_list_free_one()
{
    spell_list_node *n = setup();
    spell_list_free(&n, NULL);
    assert(n == NULL);
    printf("[PASSED] test_list_free_one\n");
}

static void
test_list_remove_head()
{
    spell_list_node *n = setup();
    spell_list_remove(&n, n, NULL);
    assert(n == NULL);
    printf("[PASSED] test_list_remove_head\n");
}

static void
test_list_get_tail()
{
    spell_list_node *n = setup();
    char *a = "a";
    char *b = "b";
    char *c = "c";
    spell_list_add(&n, a);
    spell_list_add(&n, b);
    spell_list_add(&n, c);
    spell_list_node *tail = spell_list_get_tail(n);
    assert((char *) tail->data == c);
    assert(tail->next == NULL);
    spell_list_free(&n, NULL);
    printf("[PASSED] test_list_get_tail\n");
}

static void
test_list_remove_tail()
{
    int j = 20;
    int k = 30;
    char *s = "hello";
    char *t = "world";
    spell_list_node *tail;
    spell_list_node *n = setup();
    spell_list_add(&n, &j);
    spell_list_add(&n, &k);
    spell_list_add(&n, s);
    spell_list_add(&n, t);
    tail = spell_list_get_tail(n);
    assert((char *) tail->data == t);
    spell_list_remove(&n, tail, NULL);
    tail = spell_list_get_tail(n);
    assert((char *) tail->data != t);
    assert((char *) tail->data == s);
    spell_list_free(&n, NULL);
    printf("[PASSED] test_list_remove_tail\n");

}


int main
(int argc, char **argv)
{
    test_list_init();
    test_list_add_one();
    test_list_add_multi();
    test_list_free_one();
    test_list_get_tail();
    test_list_remove_head();
    test_list_remove_tail();
}
