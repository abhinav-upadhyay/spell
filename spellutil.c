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
    size_t init_size = n > SPELL_HASHTABLE_INIT_SIZE? n + (n % 2): SPELL_HASHTABLE_INIT_SIZE;
    if (n <= 0) {
        return NULL;
    }

    spell_hashtable *hashtable = malloc(sizeof(spell_hashtable));
    if (hashtable == NULL) {
        return NULL;
    }


    hashtable->array = malloc(init_size * sizeof(spell_hashtable_entry));
    if (hashtable->array == NULL) {
        free(hashtable);
        return NULL;
    }

    memset(hashtable->array, init_size, 0);
    hashtable->size = init_size;
    hashtable->nfree = init_size;
    return hashtable;
}

static spell_hashtable_entry *
gen_hash_entry(char *key, void *val)
{
    spell_hashtable_entry *entry = malloc(sizeof(spell_hashtable_entry));
    if (entry == NULL) {
        return NULL;
    }

    spell_list_node *head = spell_list_init(val);
    if (head == NULL) {
        free(entry);
        return NULL;
    }

    entry->val_list = head;
    return entry;
}

void
spell_hashtable_add(spell_hashtable *table, char *key, void *val)
{
    if (key == NULL) {
        return;
    }

    unsigned long index = compute_hash((unsigned char *) key, table->size);
    spell_hashtable_entry *new_entry = gen_hash_entry(key, val);
    if (new_entry == NULL) {
        return;
    }
    if (table->array[index] == NULL) {
        table->array[index] = new_entry;
    } else {
         spell_list_add(&table->array[index]->val_list, new_entry);
    }
    table->nfree--;
}

static int
hash_compare_data(const void *d1, const void *d2)
{
    const keyval *kv1 = (const keyval *)d1;
    const keyval *kv2 = (const keyval *)d2;
    if (kv1->key == kv2->key) {
        return 0;
    }
    return 1;
}

void *
spell_hashtable_get(spell_hashtable *table, char *key)
{
    spell_list_node *val_list;
    spell_list_node *n;
    keyval *dummy_kv;
    keyval *val;
    unsigned long index = compute_hash((unsigned char *)key, table->size);
    if (table->array[index] == NULL) {
        return NULL;
    }

    spell_hashtable_entry *entry = table->array[index];
    val_list = entry->val_list;
    dummy_kv = malloc(sizeof(keyval));
    if (dummy_kv == NULL) {
        return NULL;
    }
    dummy_kv->key = key;
    n = spell_list_get(val_list, dummy_kv, hash_compare_data);  
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
    spell_list_node *val_list;
    spell_list_node *n;
    keyval *dummy_kv;
    keyval *val;

    unsigned long index = compute_hash((unsigned char *)key, table->size);
    if (table->array[index] == NULL) {
        return;
    }
    spell_hashtable_entry *entry = table->array[index];
    val_list = entry->val_list;
    dummy_kv = malloc(sizeof(keyval));
    if (dummy_kv == NULL) {
        return;
    }

    dummy_kv->key = key;
    n = spell_list_get(val_list, dummy_kv, hash_compare_data);
    if (n == NULL) {
        free(dummy_kv);
        return;
    }
    val = n->data;
    pfree(val);
    spell_list_remove(&val_list, n, hash_free);
    free(dummy_kv);
}

void
spell_hashtable_free(spell_hashtable *table, void (*pfree) (void *))
{

}
    


