#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
spell_list_add_head(spell_list_node **phead, void *data)
{
    spell_list_node *tail;
    spell_list_node *head;
    spell_list_node *new_node;

    head = *phead;
    new_node = malloc(sizeof(spell_list_node));
    if (new_node == NULL) {
        return -1;
    }

    new_node->next = head;
    new_node->data = data;
    *phead = new_node;
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
    
    while (iter) {
        if (compare(iter->data, data) == 0) {
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
free_entry_list(spell_list_node *head, void(*valfree) (void *))
{

    spell_list_node *n;
    spell_list_node *next;

    n = head;
    while (n) {
        next = n->next;
        keyval *kv = (keyval *) n->data;
        free(kv->key);
        if (valfree) {
            valfree(kv->val);
        }
        free(kv);
        free(n);
        n = next;
    }
}

static void
free_table_array(spell_list_node **array, size_t size, void (*pfree) (void *))
{

    for (int i = 0; i < size ; i++) {
        spell_list_node *entry = array[i];
        if (entry == NULL) {
            continue;
        }
        free_entry_list(entry, pfree);
    }
    free(array);
}

static void
resize_table(spell_hashtable *table)
{
    int i;
    size_t newsize = 2 * table->size;
    size_t oldsize = table->size;
    spell_list_node **ptr = malloc(newsize * sizeof(spell_list_node *));
    if (ptr == NULL) {
        return;
    }
    spell_list_node **old_array = table->array;
    table->array =  ptr;
    table->size = newsize;
    table->nfree = newsize;
    for (i = 0; i < newsize; i++) {
        table->array[i] = NULL;
    }

    for (i = 0; i < oldsize; i++) {
        spell_list_node *n = old_array[i];
        while(n) {
            keyval *kv = (keyval *) n->data;
            spell_hashtable_add(table, kv->key, kv->val);
            n = n->next;
        }
    }
    free_table_array(old_array, oldsize, NULL);
}

static int
hash_compare_data(const void *d1, const void *d2)
{
    const keyval *kv1 = (const keyval *)d1;
    const keyval *kv2 = (const keyval *)d2;
    return strcmp(kv1->key, kv2->key);
}

void
spell_hashtable_add(spell_hashtable *table, char *key, void *val)
{
    if (key == NULL || table == NULL) {
        return;
    }

    if (table->nfree == 0) {
        resize_table(table);
    }

    unsigned long index = compute_hash((unsigned char *) key, table->size);

    spell_list_node *entry_list = table->array[index];
    if (entry_list == NULL) {
        keyval *kv = generate_new_keyval(key, val);
        if (kv == NULL) {
            warnx("Failed to generate a new key value pair for key: %s", key);
            return;
        }
        table->array[index] = spell_list_init(kv);
        table->nfree--;
        return;
    } 

    spell_list_node *node = entry_list;
    keyval *dummy_kv = generate_new_keyval(key, NULL);
    spell_list_node *existing_key = spell_list_get(entry_list, dummy_kv, hash_compare_data);
    free(dummy_kv->key);
    free(dummy_kv);
    if (existing_key != NULL) {
        keyval *data = (keyval *) node->data;
        data->val = val;
        return;
    }

    keyval *kv = generate_new_keyval(key, val);
    if (kv == NULL) {
        warnx("Failed to generate a new key value pair for key: %s", key);
        return;
    }
    spell_list_add_head(&table->array[index], kv);
    table->nfree--;
}


void *
spell_hashtable_get(spell_hashtable *table, char *key)
{
    spell_list_node *n;
    keyval *dummy_kv;
    keyval *kv;
    unsigned long index = compute_hash((unsigned char *)key, table->size);
    spell_list_node *entry = table->array[index];
    if (entry == NULL) {
        return NULL;
    }

    dummy_kv = malloc(sizeof(keyval));
    if (dummy_kv == NULL) {
        return NULL;
    }

    dummy_kv->key = key;
    n = spell_list_get(entry, dummy_kv, hash_compare_data);  
    free(dummy_kv);
    if (n == NULL) {
        return NULL;
    }
    kv = (keyval *) n->data;
    if (kv != NULL) {
        assert(strcmp(kv->key, key) == 0);
        return kv->val;
    }
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


void
spell_hashtable_free(spell_hashtable *table, void (*pfree) (void *))
{
    if (table == NULL) {
        return;
    }
    free_table_array(table->array, table->size, pfree);
    free(table);
}

/*
 * Returns a list of all the keys stored in the table.
 * The caller needs to make sure not to free the table till
 * the table is being accessed as all the pointers in the key are
 * same as the ones stored in the table and will be invalidated
 * as soon as the table is freed. When freeing the list pass NULL
 * as the 2nd parameter to the spell_list_free function.
 */
static spell_list_node *
spell_hashtable_get_keys(spell_hashtable *table)
{
    if (table == NULL) {
        return NULL;
    }

    int i;
    spell_list_node *keylist_head = NULL;
    for (i = 0; i < table->size ; i++) {
        spell_list_node *entry = table->array[i];
        if (entry == NULL) {
            continue;
        }
        while (entry) {
            assert(entry->data);
            keyval *kv = (keyval *) entry->data;
            char *key = kv->key;
            assert(key);
            /* Caller has to make sure not to free the hashtable till
             * this list is being accessed as the pointers in the keylist
             * are pointing to the same objects which are in the table.
             */
            spell_list_add_head(&keylist_head, key);
            assert(keylist_head->data);
            entry = entry->next;
        }
    }
    return keylist_head;
}

/*
 * Returns a list of all the values stored in the table.
 * The caller needs to make sure not to free the table till
 * the table is being accessed as all the pointers in the list are
 * same as the ones stored in the table and will be invalidated
 * as soon as the table is freed. When freeing the list pass NULL
 * as the 2nd parameter to the spell_list_free function.
 */
static spell_list_node *
spell_hashtable_get_values(spell_hashtable *table)
{
    if (table == NULL) {
        return NULL;
    }

    int i;
    spell_list_node *valuelist_head = NULL;
    for (i = 0; i < table->size ; i++) {
        spell_list_node *entry = table->array[i];
        if (entry == NULL) {
            continue;
        }
        while (entry) {
            if (!entry->data) {
                entry = entry->next;
                continue;
            }
            keyval *kv = (keyval *) entry->data;
            void *data = kv->val;
            spell_list_add_head(&valuelist_head, data);
            entry = entry->next;
        }
    }
    return valuelist_head;
}

/*
 * Returns a list of all the key value pairs stored in the table.
 * The data field of the list entries is a keyval struct, where
 * the key field contains the key and the val key contains the value
 * object.
 * The caller needs to make sure not to free the table till
 * the table is being accessed as all the pointers in the list are
 * same as the ones stored in the table and will be invalidated
 * as soon as the table is freed. When freeing the list pass NULL
 * as the 2nd parameter to the spell_list_free function.
 */
static spell_list_node *
spell_hashtable_get_key_values(spell_hashtable *table) 
{
    if (table == NULL) {
        return NULL;
    }

    int i;
    spell_list_node *keyvaluelist_head = NULL;
    for (i = 0; i < table->size ; i++) {
        spell_list_node *entry = table->array[i];
        if (entry == NULL) {
            continue;
        }
        while (entry) {
            if (!entry->data) {
                entry = entry->next;
                continue;
            }
            keyval *kv = (keyval *) entry->data;
            spell_list_add_head(&keyvaluelist_head, kv);
            entry = entry->next;
        }
    }
    return keyvaluelist_head;
}

/**
 * Dump the content of the hashtable into a file in a new line separated format where each
 * line contains one key value pair.
 * The function requires a pointer to a function which can return a string representation of the
 * value object stored in the hashtable. 
 * The function also assumes that all the values stored in the table are of same type and the same
 * function can be used to get the string representation in order to write to the file. In case this
 * assumption does not hold true, do not use this function.
 */
void
spell_hashtable_dump(spell_hashtable *table, const char *filename, char *(*print) (void *))
{
    if (table == NULL) {
        return;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        warn("Failed to open file %s for writing.", filename);
        return;
    }

    spell_list_node *keyvaluelist_head = spell_hashtable_get_key_values(table);
    if (keyvaluelist_head == NULL) {
        fclose(file);
        warnx("Failed to read all the key value pairs from the table.");
        return;
    }

    spell_list_node *node = keyvaluelist_head;
    while (node != NULL) {
        keyval *kv = node->data;
        if (kv == NULL) {
            node = node->next;
            continue;
        }
        char *val = print(kv->val);
        fprintf(file, "%s %s\n", kv->key, val);
        free(val);
        node = node->next;
    }
    fclose(file);
    spell_list_free(&keyvaluelist_head, NULL);
}
