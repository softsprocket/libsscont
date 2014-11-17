
/*
*    libsscont - a library of software containers
*
*    Copyright (C) 2014 Gregory Ralph Martin
*    info at softsprocket dot com
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Lesser General Public
*    License as published by the Free Software Foundation; either
*    version 2.1 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public
*    License along with this library; if not, write to the Free Software
*    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
*    USA
*/

#include "container.h"
#include "debug_utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
		+(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

// see http://www.azillionmonkeys.com/qed/hash.html 
// Paul Hsieh
uint32_t SuperFastHash (const char * data, int len) {
	uint32_t hash = len, tmp;
	int rem;

	if (len <= 0 || data == NULL) return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (;len > 0; len--) {
		hash  += get16bits (data);
		tmp    = (get16bits (data+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 2*sizeof (uint16_t);
		hash  += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
		case 3: hash += get16bits (data);
			hash ^= hash << 16;
			hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
			hash += hash >> 11;
			break;
		case 2: hash += get16bits (data);
			hash ^= hash << 11;
			hash += hash >> 17;
			break;
		case 1: hash += (signed char)*data;
			hash ^= hash << 10;
			hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}


hash_table* hash_table_create (size_t size_table) {
	size_t bucket_size = 10;

	hash_table* ht = malloc (sizeof (hash_table));
	if (ht == NULL) {
		PERR ("malloc");
		return NULL;
	}

	ht->buckets = malloc (sizeof (hash_bucket*) * size_table);
	if (ht->buckets == NULL) {
		PERR ("malloc");
		free (ht);
		return NULL;
	}

	ht->size = size_table;

	for (size_t i = 0; i < size_table; ++i) {
		ht->buckets[i] = malloc (sizeof (hash_bucket));
		if (ht->buckets[i] == NULL) {
			PERR ("malloc");
			for (size_t j = 0; j < i; ++j) {
				free (ht->buckets[j]->entries);
				free (ht->buckets[j]);
			}
			free (ht->buckets);
			free (ht);

			return NULL;
		}

		ht->buckets[i]->entries = malloc (sizeof (hash_entry*) * bucket_size);
		if (ht->buckets[i]->entries == NULL) {
			PERR ("malloc");
			for (size_t j = 0; j < i; ++j) {
				free (ht->buckets[j]->entries);
				free (ht->buckets[j]);
			}
			free (ht->buckets[i]);
			free (ht->buckets);
			free (ht);

			return NULL;
		}

		ht->buckets[i]->size = bucket_size;	
		ht->buckets[i]->count = 0;
	}

	return ht;

}

hash_entry* hash_table_put (hash_table* ht, char* key, void* value) {
	uint32_t hash = SuperFastHash (key, strlen (key));

	size_t pos = hash % ht->size;

	int bpos = ht->buckets[pos]->count;

	if (bpos == (ht->buckets[pos]->size - 1)) {
		int new_size = ht->buckets[pos]->size * 2;
		void* tmp;
		if ((tmp = realloc (ht->buckets[pos], new_size * sizeof (hash_entry*))) == NULL) {
			PERR ("realloc");
			return NULL;
		}
		ht->buckets[pos] = tmp;
		ht->buckets[pos]->size = new_size;	
	}

	for (int i = 0; i < bpos; ++i) {
		if (ht->buckets[pos]->entries[i] == NULL) {
			bpos = i;
		       	break;
		}	
	}

	ht->buckets[pos]->entries[bpos] = malloc (sizeof (hash_entry));
	if (ht->buckets[pos]->entries[bpos] == NULL) {
		PERR ("malloc");
		return NULL;
	}

	ht->buckets[pos]->count++;

	ht->buckets[pos]->entries[bpos]->hash = hash;
	ht->buckets[pos]->entries[bpos]->key = malloc (strlen (key) + 1);
	if (ht->buckets[pos]->entries[bpos]->key == NULL) {
		PERR ("malloc");
		free (ht->buckets[pos]->entries[bpos]);
		return NULL;
	}

	strcpy (ht->buckets[pos]->entries[bpos]->key, key);
	ht->buckets[pos]->entries[bpos]->value = value;

	return ht->buckets[pos]->entries[bpos];
}

void* hash_table_get (hash_table* ht, char* key) {
	uint32_t hash = SuperFastHash (key, strlen (key));

	size_t pos = hash % ht->size;
	
	int bpos = ht->buckets[pos]->count;

	for (int i = 0; i < bpos; ++i) {
		if (ht->buckets[pos]->entries[i] != NULL) {
			if (strcmp (ht->buckets[pos]->entries[i]->key, key) == 0) {
				bpos = i;
				break;
			}
		}
	}
	
	return bpos == ht->buckets[pos]->count ? NULL : ht->buckets[pos]->entries[bpos]->value;
}

auto_array* hash_table_get_all (hash_table* ht, char* key) {
	uint32_t hash = SuperFastHash (key, strlen (key));

	size_t pos = hash % ht->size;
	
	int bpos = ht->buckets[pos]->count;

	auto_array* aa = auto_array_create (bpos);
	if (aa == NULL) {
		PMSG ("auto_array_create failed");
		return NULL;
	}

	for (int i = 0; i < bpos; ++i) {
		if (ht->buckets[pos]->entries[i] != NULL) {
			if (strcmp (ht->buckets[pos]->entries[i]->key, key) == 0) {
				auto_array_add (aa, ht->buckets[pos]->entries[i]->value);
			}
		}
	}
	
	return aa;
}

void* hash_table_remove (hash_table* ht, char* key) {
	
	uint32_t hash = SuperFastHash (key, strlen (key));

	size_t pos = hash % ht->size;
	
	int bpos = ht->buckets[pos]->count;

	void* entry_value = NULL;

	for (int i = 0; i < bpos; ++i) {
		if (ht->buckets[pos]->entries[i] != NULL) {
			if (strcmp (ht->buckets[pos]->entries[i]->key, key) == 0) {
				entry_value = ht->buckets[pos]->entries[i]->value; 
				free (ht->buckets[pos]->entries[i]->key);
				free (ht->buckets[pos]->entries[i]);
				ht->buckets[pos]->entries[i] = NULL;
				ht->buckets[pos]->count--;
				break;
			}
		}
	}
	

	return entry_value;
}

void hash_table_delete (hash_table* ht, void (*delete_value)(void*)) {

	for (size_t i = 0; i < ht->size; ++i) {
		size_t count = ht->buckets[i]->count;
		int pos = 0;

		while (count) {
			
			if (ht->buckets[i]->entries[pos] != NULL) {
				free (ht->buckets[i]->entries[pos]->key);
				if (delete_value != NULL) {
					delete_value (ht->buckets[i]->entries[pos]->value);
				}
				
				free (ht->buckets[i]->entries[pos]);
				--count;
			}

			++pos;
		}

		free (ht->buckets[i]->entries);
		free (ht->buckets[i]);
	}

		
	free (ht->buckets);
	free (ht);
	ht = NULL;
}

auto_array* hash_table_keys (hash_table* ht) {
	size_t num_keys_estimate = 1000;

	auto_array* aa = auto_array_create (num_keys_estimate);
	if (aa == NULL) {
		PERR ("auto_array");
		return NULL;
	}

	for (int i = 0; i < ht->size; ++i) {
		for (int j = 0; j < ht->buckets[i]->count; ++j) {
			if (ht->buckets[i]->entries[j] != NULL) {
				auto_array_add (aa, ht->buckets[i]->entries[j]->key);
			}
		}	
	}

	return aa;
}


