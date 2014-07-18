#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "spellutil.h"

static void
test_list_init()
{
    spell_list_node *n = NULL;
    printf("[PASSED] test_list_init\n");
    spell_list_free(&n, NULL);
}

static void
test_list_add_one()
{
    int j = 20;
    spell_list_node *n = NULL;
    int status = spell_list_add_head(&n, &j);
    assert(n->next == NULL);
    assert(*(int *)n->data == j);
    printf("[PASSED] test_list_add_one\n");
    spell_list_free(&n, NULL);
}

static void
test_list_add_head_multi()
{
    int i = 0;
    int j = 1;
    int k = 2;
    int l = 3;

    spell_list_node *head = spell_list_init(&i);
    spell_list_add_head(&head, &j);
    spell_list_add_head(&head, &k);
    spell_list_add_head(&head, &l);
    assert(*(int *) head->data == l);
    assert(* (int *) head->next->data == k);
    assert(* (int *) head->next->next->data == j);
    assert(* (int *) head->next->next->next->data == i);
    spell_list_free(&head, NULL);
    printf("[PASSED] test_list_add_head_multi\n");
}

static void
test_list_free_one()
{
    spell_list_node *n = NULL;
    spell_list_free(&n, NULL);
    assert(n == NULL);
    printf("[PASSED] test_list_free_one\n");
}

static void
test_list_remove_head()
{
    spell_list_node *n = NULL;
    spell_list_remove(&n, n, NULL);
    assert(n == NULL);
    printf("[PASSED] test_list_remove_head\n");
}

static void
test_list_get_tail()
{
    spell_list_node *n = NULL;
    char *a = "a";
    char *b = "b";
    char *c = "c";
    spell_list_add_head(&n, a);
    spell_list_add_head(&n, b);
    spell_list_add_head(&n, c);
    spell_list_node *tail = spell_list_get_tail(n);
    assert(tail->data == a);
    assert(tail->next == NULL);
    spell_list_free(&n, NULL);
    printf("[PASSED] test_list_get_tail\n");
}

static int
node_compare(const void *d1, const void *d2)
{
    const char *s1 = (const char *) d1;
    const char *s2 = (const char *) d2;
    return strcmp(s1, s2);
}

static void
test_list_get_middle()
{
    spell_list_node *n = NULL;
    char *a = "a";
    char *b = "b";
    char *c = "c";
    spell_list_add_head(&n, a);
    spell_list_add_head(&n, b);
    spell_list_add_head(&n, c);
    spell_list_node *mid_node = spell_list_get(n, "a", node_compare);
    assert(mid_node != NULL);
    spell_list_free(&n, NULL);
    printf("[PASSED] test_list_get_middle\n");
}

static void
test_list_remove_tail()
{
    int j = 20;
    int k = 30;
    char *s = "hello";
    char *t = "world";
    spell_list_node *tail;
    spell_list_node *n = NULL;
    spell_list_add_head(&n, &j);
    spell_list_add_head(&n, &k);
    spell_list_add_head(&n, s);
    spell_list_add_head(&n, t);
    tail = spell_list_get_tail(n);
    assert(*(int *) tail->data == j);
    spell_list_remove(&n, tail, NULL);
    tail = spell_list_get_tail(n);
    assert(*(int *) tail->data == k);
    spell_list_free(&n, NULL);
    printf("[PASSED] test_list_remove_tail\n");

}

static void
test_list_remove_middle()
{
    spell_list_node *to_be_deleted;
    spell_list_node *prev;
    spell_list_node *next;
    spell_list_node *n = NULL;
    char *a = "a";
    char *b = "b";
    char *c = "c";
    char *d = "d";
    spell_list_add_head(&n, a);
    spell_list_add_head(&n, b);
    spell_list_add_head(&n, c);
    spell_list_add_head(&n, d);
    prev = n;
    to_be_deleted = prev->next;
    next = to_be_deleted->next;
    while ((char *) to_be_deleted->data != c) {
        prev = to_be_deleted;
        to_be_deleted = to_be_deleted->next;
        next = to_be_deleted->next;
    }
    spell_list_remove(&n, to_be_deleted, NULL);
    assert(prev->next == next);
    spell_list_free(&n, NULL);
    printf("[PASSED] test_list_remove_middle\n");
}


int main
(int argc, char **argv)
{
    test_list_init();
    test_list_add_one();
    test_list_free_one();
    test_list_get_tail();
    test_list_get_middle();
    test_list_remove_head();
    test_list_remove_tail();
    test_list_remove_middle();
}
