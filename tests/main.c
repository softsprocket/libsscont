#include "debug_utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "container.h"

int auto_string_test ();
int auto_array_test ();
int hash_table_test ();
int set_test ();

int main (int argc, char** argv) {
	int rv = EXIT_SUCCESS;

	rv = rv | auto_string_test ();
	rv = rv | auto_array_test ();
	rv = rv | hash_table_test ();
	rv = rv | set_test ();

	return rv;
}


int auto_string_test () {
	char* str_parts[7] = {
		"Hello",
		",",
		" ",
		"Brave",
		" ",
		"World",
		"!"
	};

	char* test_str = "Hello, Brave World!";

	auto_string* as = auto_string_create (10);

	if (as == NULL) {
		PMSG ("auto_string allocation failed");
		return EXIT_FAILURE;
	}

	if (as->count != 0) {
		PMSG ("auto_string count intialization failure");
	       	return EXIT_FAILURE;
	}

	if (as->size != 10) {
		PMSG ("auto_string size intialization failure");
	       	return EXIT_FAILURE;
	}

	for (int i = 0 ; i < 7; ++i) {
		auto_string_append (as, str_parts[i]);
	}

	if (strcmp (as->buf, test_str) != 0) {
		fprintf (stderr, "<test: %s>\n", test_str);
		fprintf (stderr, "<auto: %s>\n", as->buf);
		PMSG ("auto_string strcmp test failed");
		return EXIT_FAILURE;
	}

	if (auto_string_length (as) != strlen (test_str)) {
		fprintf (stderr, "%lu != %lu\n", auto_string_length (as), strlen (test_str));
		PMSG ("auto_string_length test failed");
		return EXIT_FAILURE;	
	}

	auto_string_delete (as);

	printf ("auto_string tests pass\n");

	return EXIT_SUCCESS;
}

int auto_array_test () {
	auto_array* aa = auto_array_create (10);

	if (aa == NULL) {
		PMSG ("auto_array_create returned NULL");
		return EXIT_FAILURE;
	}

	if (aa->size != 10) {
		PMSG ("auto_array initialization: wrong size value");
		return EXIT_FAILURE;
	}

	if (aa->count != 0) {
		PMSG ("auto_array initialization: wrong count value");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < 1000; ++i) {
		int* ip = malloc (sizeof (int));
		if (ip == NULL) {
			PERR ("malloc");
			exit (EXIT_FAILURE);
		}

		*ip = i;

		auto_array_add (aa, ip);
	}

	if (aa->count != 1000) {
		PDEC ();
		fprintf (stderr, "auto_array_add: aa->count - %lu != 1000\n", aa->count);
		return EXIT_FAILURE;
	}

	for (int i = 0; i < 1000; ++i) {
		int* ip = auto_array_get (aa, i);

		if (*ip != i) {
			PDEC ();
			fprintf (stderr, "auto_array_get: %d != %d\n", *ip, i);
			return EXIT_FAILURE;
		}

		free (ip);
		
		ip = malloc (sizeof (int));
		if (ip == NULL) {
			PERR ("malloc");
			exit (EXIT_FAILURE);
		}
		
		*ip = 2 * i;
		ssize_t pos = 0;
		if ((pos = auto_array_put (aa, i, ip)) != 1000) {
			PDEC ();
			fprintf (stderr, "auto_array_put: put at %d returned %ld\n", i, pos);
			return EXIT_FAILURE;
		}
	}

	for (int i = 0; i < 1000; ++i) {
		int* ip = auto_array_get (aa, i);

		if (*ip != (i * 2)) {
			PDEC ();
			fprintf (stderr, "auto_array_put: %d != %d\n", *ip, 2 * i);
			return EXIT_FAILURE;
		}
	}	

	for (int i = 500; i < 600; i += 10) {
		int* ip = malloc (sizeof (int));
		if (ip == NULL) {
			PERR ("malloc");
			exit (EXIT_FAILURE);
		}

		*ip = aa->count;
		ssize_t len = auto_array_insert (aa, i, ip);
		if (len != (*ip + 1)) {
			PDEC ();
			fprintf (stderr, "auto_array_insert: %lu != %d\n", len, *ip + 1);
			return EXIT_FAILURE;
		}

		int* rv = auto_array_get (aa, i);
		if (*rv != *ip) {
			PDEC ();
			fprintf (stderr, "auto_array_insert: %d != %d\n", *rv, *ip);
			return EXIT_FAILURE;
		}
	}

	int* l = auto_array_last (aa);
	if (*l != 1998) {
		PDEC ();
		fprintf (stderr, "auto_array_last: %d != 1998\n", *l);
		return EXIT_FAILURE;
	}
	

	int* rl = auto_array_remove (aa, aa->count - 1);
      	if (rl == NULL) {
		PDEC ();
		fprintf (stderr, "auto_array_remove returned NULL\n");
		return EXIT_FAILURE;
	}

	if (*l != *rl) {
		PDEC ();
		fprintf (stderr, "auto_array_remove: %d != %d\n", *l, *rl);
		return EXIT_FAILURE;
	}

	free (rl);

	l = auto_array_last (aa);
	if (*l != 1996) {
		PDEC ();
		fprintf (stderr, "auto_array_last: %d != 1996\n", *l);
		return EXIT_FAILURE;
	}

	auto_array_delete (aa, free);

	printf ("auto_array tests pass\n");

	return EXIT_SUCCESS;
}

int hash_table_test () {
	hash_table* ht = hash_table_create (10);
	if (ht == NULL) {
		PMSG ("hash_table_create: returned NULL");
		return EXIT_FAILURE;
	}

	if (ht->size != 10) {
		PDEC();
		fprintf (stderr, "hash_table_create initialization: ht->size %lu != 10\n", ht->size);
		return EXIT_FAILURE;
	}

	char* key_values[6][2] = {
		{ "red", "Roses are red" },
		{ "red", "Apples are red" },
		{ "red", "Books are read" },
		{ "blue", "The sky is blue" },
		{ "green", "Grass is green" },
		{ "green", "Avacadoes are green"}
	};

	for (int i = 0; i < 6; ++i) {
		hash_entry* he = hash_table_put (ht, key_values[i][0], key_values[i][1]);
		if (he == NULL) {
			PMSG ("hash_table_put: returned NULL");
			return EXIT_FAILURE;
		}
		
		if (strcmp (he->key, key_values[i][0]) != 0) {
			PDEC ();
			fprintf (stderr, "hash_table_put: hash_entry key %s != %s\n", he->key, key_values[i][0]);
			return EXIT_FAILURE;
		}

	}

	auto_array* keys = hash_table_keys (ht);
	if (keys->count != 6) {
		PDEC ();
		fprintf (stderr, "hash_table_keys count %lu != 6\n", keys->count);
		return EXIT_FAILURE;
	}

	auto_array_delete (keys, NULL);

	auto_array* reds = hash_table_get_all (ht, "red");
	if (reds == NULL) {
		PMSG ("hash_table_get_all: returned NULL");
		return EXIT_FAILURE;
	}

	if (reds->count != 3) {
		PDEC ();
		fprintf (stderr, "hash_table_get_all \"red\" count %lu != 3\n", reds->count);
		return EXIT_FAILURE;
	}

	for (int i = 0; i < reds->count; ++i) {
		char* s = auto_array_get(reds, i);
		if (strcmp (s, key_values[i][1]) != 0) {
			PDEC ();
			fprintf (stderr, "hash_table_get_all: %s != %s\n", s, key_values[i][1]);
			return EXIT_FAILURE;
		}
	}

	auto_array_delete (reds, NULL);

	char* blue_str = hash_table_get (ht, "blue");
	if (blue_str == NULL) {
		PMSG ("hash_table_get: returned NULL");
		return EXIT_FAILURE;
	}

	if (strcmp (blue_str, key_values[3][1]) != 0) {
		PDEC ();
		fprintf (stderr, "hash_table_get: %s != %s\n", blue_str, key_values[3][1]);
		return EXIT_FAILURE;
	}

	blue_str = hash_table_remove (ht, "blue");
	if (blue_str == NULL) {
		PMSG ("hash_table_remove: returned NULL");
		return EXIT_FAILURE;
	}

	if (strcmp (blue_str, key_values[3][1]) != 0) {
		PDEC ();
		fprintf (stderr, "hash_table_remove: %s != %s\n", blue_str, key_values[3][1]);
		return EXIT_FAILURE;
	}

	blue_str = hash_table_get (ht, "blue");
	if (blue_str != NULL) {
		PDEC ();
		fprintf (stderr, "hash_table_remove:  get blue returned %s\n", blue_str);
		return EXIT_FAILURE;
	}

	hash_table_delete (ht, NULL);

	printf ("hash_table tests pass\n");

	return EXIT_SUCCESS;
}

int equals (void* this, void* that) {
	int* l = this;
	int* r = that;

	return *l == *r;
}

void dset (void* v) {
	set_delete (v, NULL);
}

int str_equals (void* this, void* that) {
	return strcmp (this, that) == 0;
}

int set_test () {
	
	set* s = set_create (3, equals);
	if (s == NULL) {
		PMSG ("set_create returned NULL");
		exit (EXIT_FAILURE);
	}
	
	if (s->size != 3) {
		PMSG ("set initialization: wrong size value");
		return EXIT_FAILURE;
	}

	if (s->count != 0) {
		PMSG ("set initialization: wrong count value");
		return EXIT_FAILURE;
	}

	int pi = 2;
	ssize_t ii = set_get_item_index (s, &pi);

	if (ii != -1) {
		PDEC ();
		fprintf (stderr, "set_get_item_index: %lu != -1\n", ii);
		return EXIT_FAILURE;
	}

	for (int i= 1; i < 4; ++i) {
		int* ip = malloc (sizeof (int));
		if (ip == NULL) {
			PERR ("malloc");
			exit (EXIT_FAILURE);
		}

		*ip = i;

		ssize_t c = 0;
		if ((c = set_add_item (s, ip)) != i) {
			PDEC ();
			fprintf (stderr, "set_add_item returned %lu, not %d\n", c, i);
			return EXIT_FAILURE;		
		}
	}

	if (s->count != 3) {
		PDEC ();
		fprintf (stderr, "set_add_item: count %lu != 3\n", s->count);
		return EXIT_FAILURE;
	}

	int* ip = set_get_item (s, 1);
	ssize_t item_index = set_get_item_index (s, ip);

	if (item_index != 1) {
		PDEC ();
		fprintf (stderr, "set_get_item_index: %lu != 1\n", item_index);
		return EXIT_FAILURE;
	}

	set* all = set_all_subsets (s);
	if (all == NULL) {
		PMSG ("set_all_subsets returned NULL\n");
		return EXIT_FAILURE;
	}

	if (all->count != 8) {
		PDEC ();
		fprintf (stderr, "set_all_subsets count %lu != 8\n", all->count);
		return EXIT_FAILURE;
	}

	int setof[8][3] = {
		{ },
		{ 1 },
		{ 2 },
		{ 1, 2 },
               	{ 3 },
		{ 1, 3 },
		{ 2, 3 },
		{ 1, 2, 3 }
	};

	for (int i = 0; i < all->count; ++i) {
		set* n = set_get_item (all, i);
		for (int j = 0; j < n->count; ++j) {
			int* item = set_get_item (n, j);
			if (*item != setof [i][j]) {
				PDEC ();
				fprintf (stderr, "set_all_subsets: %d != %d\n", *item, setof [i][j]);
				return EXIT_FAILURE;
			}
		}

	}

	set_delete (all, dset);
	set_delete (s, free);

	char* nums[5] = {
		"one", "two", "three", "four", "five"
	};

	set* str_set = set_create (5, str_equals);
	if (str_set == NULL) {
		PMSG ("set_create returned NULL");
		return EXIT_FAILURE;
	}

	ssize_t rc = set_add_items (str_set, (void**) nums, 5);
	if (rc != 5) {
		PDEC ();
		fprintf (stderr, "set_add_items: returned %lu != 5\n", rc);
		return EXIT_FAILURE;
	}


	for (int i = 0; i < str_set->count; ++i) {
		char* sss = set_get_item (str_set, i);
		char* sn = nums[i];
		if (strcmp (sss, sn) != 0) {
			PDEC ();
			fprintf (stderr, "set_add_items: %s != %s\n", sss, sn);
		       	return EXIT_FAILURE;	
		}
	}

	char* nums2[4] = {
		"three", "five", "seven", "nine"
	};

	set* str2_set = set_create (4, str_equals);
	if (str2_set == NULL) {
		PMSG ("set_create returned NULL");
		return EXIT_FAILURE;
	}

	if (set_add_items (str2_set, (void**) nums2, 4) == -1) {
		PMSG ("set_add_items return -1");
		return EXIT_FAILURE;
	}
	
	set* uni = set_union (str_set, str2_set);
	if (uni == NULL) {
		PMSG ("set_union returned NULL");
		return EXIT_FAILURE;
	}

	char* exp_uni[7] = {
		"one", "two", "three", "four", "five", "seven", "nine"
	};

	for (int i = 0; i < uni->count; ++i) {
		char* it = set_get_item (uni, i);
		char* xit = exp_uni[i];
		if (strcmp (it, xit) != 0) {
			PDEC ();
			fprintf (stderr, "set_union: %s != %s\n", it, xit);
			return EXIT_FAILURE;
		}
	}

	set_delete (uni, NULL);

	set* inter = set_intersection (str_set, str2_set);
	if (inter == NULL) {
		PMSG ("set_intersection return NULL");
		return EXIT_FAILURE;
	}

	char* exp_inter[4] = {
		"three", "five", "seven", "nine"
	};

	for (int i = 0; i < inter->count; ++i) {
		char* is = set_get_item (inter, i);
		char* exi = exp_inter[i];
		if (strcmp (is, exi) != 0) {
			PDEC ();
			fprintf (stderr, "set_intersection: %s != %s\n", is, exi);
			return EXIT_FAILURE;
		}
	}

	set_delete (inter, NULL);
	set_delete (str_set, NULL);
	set_delete (str2_set, NULL);


	printf ("set tests pass\n");

	return EXIT_SUCCESS;
}

