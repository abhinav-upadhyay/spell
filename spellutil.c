#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
spell_list_get(spell_list_node *head, void *data, int (*compare) (const void*, const void*))
{
    spell_list_node *iter = head;
    if (head == NULL) {
        return NULL;
    }
    
    if (!compare(head->data, data)) {
        return head;
    }

    while (iter->next) {
        if (!compare(iter->data, data)) {
            return iter;
        }
        iter = iter->next;
    }
    return NULL;
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

unsigned long
compute_hash(unsigned char *str, size_t bound)
{
	unsigned long hash = 5381;
	int c; 
	while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash*33 + c
    }
	return hash % bound;
}

spell_hashtable *
spell_hashtable_init(size_t n)
{
    if (n <= 0) {
        return NULL;
    }

    size_t init_size = n > SPELL_HASHTABLE_INIT_SIZE? n + (n % 2): SPELL_HASHTABLE_INIT_SIZE;

    spell_hashtable *hashtable = malloc(sizeof(spell_hashtable));
    if (hashtable == NULL) {
        return NULL;
    }

    hashtable->array = malloc(init_size * sizeof(spell_list_node *));
    if (hashtable->array == NULL) {
        free(hashtable);
        return NULL;
    }

    memset(hashtable->array, 0, init_size * sizeof(spell_list_node *));
    hashtable->size = init_size;
    hashtable->nfree = init_size;
    return hashtable;
}

static keyval*
generate_new_keyval(char *key, void *val)
{
    keyval *kv = malloc(sizeof(keyval));
    if (kv == NULL) {
        return NULL;
    }
    kv->key = strdup(key);
    kv->val = val;
    return kv;
}

static void
resize_table(spell_hashtable *table)
{
    int i;
    spell_list_node **ptr = realloc(table->array, 2 * table->size * sizeof(spell_list_node *));
    if (ptr == NULL) {
        return;
    }
    table->array =  ptr;
    for (i = table->size; i < 2 * table->size; i++) {
        table->array[i] = NULL;
    }
    table->nfree = table->size;
    table->size *= 2;
}

void
spell_hashtable_add(spell_hashtable *table, char *key, void *val)
{
    if (key == NULL) {
        return;
    }

    if (table->nfree == 0) {
        resize_table(table);
    }

    unsigned long index = compute_hash((unsigned char *) key, table->size);
    keyval *kv = generate_new_keyval(key, val);
    if (kv == NULL) {
        return;
    }

    if (table->array[index] == NULL) {
        table->array[index] = spell_list_init(kv);
    } else {
         spell_list_add(&table->array[index], kv);
    }
    table->nfree--;
}

static int
hash_compare_data(const void *d1, const void *d2)
{
    const keyval *kv1 = (const keyval *)d1;
    const keyval *kv2 = (const keyval *)d2;
    if (strcmp(kv1->key, kv2->key) == 0) {
        return 0;
    }
    return 1;
}

void *
spell_hashtable_get(spell_hashtable *table, char *key)
{
    spell_list_node *n;
    keyval *dummy_kv;
    keyval *val;
    unsigned long index = compute_hash((unsigned char *)key, table->size);
    if (table->array[index] == NULL) {
        return NULL;
    }

    spell_list_node *entry = table->array[index];
    dummy_kv = malloc(sizeof(keyval));
    if (dummy_kv == NULL) {
        return NULL;
    }
    dummy_kv->key = key;
    n = spell_list_get(entry, dummy_kv, hash_compare_data);  
    if (n == NULL) {
        free(dummy_kv);
        return NULL;
    }
    val = n->data;
    if (val != NULL) {
        free(dummy_kv);
        return val->val;
    }
    free(dummy_kv);
    return NULL;
}

static void
hash_free(void *keyval_pair)
{
    keyval *kv = (keyval *) keyval_pair;
    free(kv->key);
}

void
spell_hashtable_remove(spell_hashtable *table, char *key, void (*pfree) (void *))
{
    spell_list_node *n;
    keyval *dummy_kv;
    keyval *val;

    unsigned long index = compute_hash((unsigned char *)key, table->size);
    if (table->array[index] == NULL) {
        return;
    }
    spell_list_node *entry = table->array[index];
    dummy_kv = malloc(sizeof(keyval));
    if (dummy_kv == NULL) {
        return;
    }

    dummy_kv->key = key;
    n = spell_list_get(entry, dummy_kv, hash_compare_data);
    if (n == NULL) {
        free(dummy_kv);
        return;
    }
    val = n->data;
    pfree(val);
    spell_list_remove(&entry, n, hash_free);
    free(dummy_kv);
}

static void
free_keyval(void *d)
{
    keyval *kv = (keyval *) d;
    free(kv->key);
}

static void
free_entry_list(spell_list_node **phead, void(*valfree) (void *))
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
        keyval *kv = (keyval *) n->data;
        free(kv->key);
        if (valfree)
            valfree(kv->val);
        free(kv);
        free(n);
        n = next;
    }
    *phead = NULL;
}

void
spell_hashtable_free(spell_hashtable *table, void (*pfree) (void *))
{
    if (table == NULL) {
        return;
    }
    int i;
    for (i = 0; i < table->size ; i++) {
        spell_list_node *entry = table->array[i];
        if (entry == NULL) {
            continue;
        }
        free_entry_list(&entry, pfree);
    }
    free(table->array);
    free(table);
}
