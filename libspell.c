#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libspell.h"
#include "spellutil.h"

#define N_PERMUTATIONS(n) 54 * n +25 

typedef struct set {
	char *a;
	char *b;
} set;


static spell_list_node * 
edits1 (char *word)
{
	int i;
	int len_a;
	int len_b;
	char alphabet;
	int n = strlen(word);
	set splits[n + 1];
    char *candidate;
	
	/* calculate number of possible permutations and allocate memory */
	size_t size = N_PERMUTATIONS(n); 
	char **candidates = malloc (size * sizeof(char *));
    spell_list_node *candidates_list = spell_list_init(NULL);

	/* Start by generating a split up of the characters in the word */
	for (i = 0; i < n + 1; i++) {
		splits[i].a = (char *) malloc(i + 1);
		splits[i].b = (char *) malloc(n - i + 1);
		memcpy(splits[i].a, word, i);
		memcpy(splits[i].b, word + i, n - i + 1);
		splits[i].a[i] = 0;
	}

	/* Now generate all the permutations at maximum edit distance of 1.
	 * counter keeps track of the current index position in the array candidates
	 * where the next permutation needs to be stored.
	 */
	for (i = 0; i < n + 1; i++) {
		len_a = strlen(splits[i].a);
		len_b = strlen(splits[i].b);
		assert(len_a + len_b == n);

		/* Deletes */
		if (i < n) {
			candidate = malloc(n);
			memcpy(candidate, splits[i].a, len_a);
			if (len_b -1 > 0)
				memcpy(candidate + len_a , splits[i].b + 1, len_b - 1);
			candidate[n - 1] =0;
            spell_list_add(&candidates_list, candidate);
		}

		/* Transposes */
		if (i < n - 1) {
			candidate = malloc(n + 1);
			memcpy(candidate, splits[i].a, len_a);
			if (len_b >= 1)
				memcpy(candidate + len_a, splits[i].b + 1, 1);
			if (len_b >= 1)
				memcpy(candidate + len_a + 1, splits[i].b, 1);
			if (len_b >= 2)
				memcpy(candidate + len_a + 2, splits[i].b + 2, len_b - 2);
			candidate[n] = 0;
            spell_list_add(&candidates_list, candidate);
		}

		/* For replaces and inserts, run a loop from 'a' to 'z' */
		for (alphabet = 'a'; alphabet <= 'z'; alphabet++) {
			/* Replaces */
			if (i < n) {
				candidate = malloc(n + 1);
				memcpy(candidate, splits[i].a, len_a);
				memcpy(candidate + len_a, &alphabet, 1);
				if (len_b - 1 >= 1)
					memcpy(candidate + len_a + 1, splits[i].b + 1, len_b - 1);
				candidate[n] = 0;
                spell_list_add(&candidates_list, candidate);
			}

			/* Inserts */
			candidate = malloc(n + 2);
			memcpy(candidate, splits[i].a, len_a);
			memcpy(candidate + len_a, &alphabet, 1);
			if (len_b >=1)
				memcpy(candidate + len_a + 1, splits[i].b, len_b);
			candidate[n + 1] = 0;
            spell_list_add(&candidates_list, candidate);
		}
	}

    for (i = 0; i < n + 1; i++) {
        free(splits[i].a);
        free(splits[i].b);
    }
	return candidates_list;
}
