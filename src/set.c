
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

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

set* set_create (size_t size, int (*equals) (void*, void*)) {
	set* s = malloc (sizeof (set));

	if (s == NULL) {
		PERR ("malloc");
		return NULL;
	}

	s->data = malloc (sizeof (void*) * size);

	if (s->data == NULL) {
		PERR ("malloc");
		free (s);

		return NULL;
	}

	s->size = size;
	s->count = 0;
	s->equals = equals;

	return s;
}

ssize_t set_get_item_index (set* s, void* item) {
	for (int i = 0; i < s->count; ++i) {
		void* data = s->data[i];
		if (s->equals (data, item)) {
			return i;
		}
	}

	return -1;
}

ssize_t set_add_item (set* s, void* item) {

	if (s->count >= s->size) {
		PMSG ("Item would cause overflow");
		return -1;
	}

	ssize_t existing = set_get_item_index (s, item);
	if (existing != -1) {
		return existing;
	}

	s->data[s->count] = item;
	s->count++;

	return s->count;	
}

ssize_t set_add_items (set* s, void** items, size_t size) {

	if (size > (s->size - s->count)) {
		PMSG ("Size of items would cause overflow");
		return -1;
	}

	for (int i = 0; i < size; ++i) {
		 set_add_item (s, items[i]);
	}

	return s->count;
}

int all_set_equ (void* l, void* r) {
	return 0;
}

set* set_all_subsets (set* s) {
	size_t num_sets = pow (2, s->count);

	set* all_sets = set_create (num_sets, all_set_equ);

	set* next_set = set_create (0, s->equals);
	set_add_item (all_sets, next_set);

	for (int i = 0; i < s->count; ++i) {
		void* next_item = set_get_item (s, i);

		int count = all_sets->count;
		for (int j = 0; j < count; ++j) {

			set* to_add_to = set_get_item (all_sets, j);

			int new_set_count = to_add_to->count + 1;

			set* new_set = set_create (new_set_count, s->equals);

			if (new_set_count > 1) {
				set_add_items (new_set, to_add_to->data, to_add_to->count);
			}
			
			set_add_item (new_set, next_item);

			set_add_item (all_sets, new_set);
		}
	}

	return all_sets;	
}

void* set_get_item (set* s, size_t pos) {
	if (pos >= s->count) {
		return NULL;
	}

	return s->data[pos];
}

set* set_intersection (set* s, set* other) {
	auto_array* rvals = auto_array_create (s->count + other->count);
	if (rvals == NULL) {
		PMSG ("auto_array");
		return NULL;
	}

	auto_array* other_vals = auto_array_create (other->count);
	if (other_vals == NULL) {
		PMSG ("auto_array");
		return NULL;
	}

	memcpy (other_vals->data, other->data, sizeof (void*) * other->count);
	other_vals->count = other->count;

	for (int i = 0; i < s->count; ++i) {
		void* item = set_get_item (s, i);
		ssize_t ind = set_get_item_index (other, item);
		if (ind != -1) {
			auto_array_add (rvals, item);
			auto_array_put (other_vals, ind, NULL);
		}
	}

	for (int i = 0; i < other_vals->count; ++i) {
		void* item = auto_array_get (other_vals, i);
		if (item == NULL) {
			continue;
		}

		auto_array_add (rvals, item);
	}	

	if (rvals->count == 0) {
		return NULL;
	}

	set* rset = set_create (rvals->count, s->equals);
	memcpy (rset->data, rvals->data, sizeof (void*) * rvals->count);
        rset->count = rvals->count;

	auto_array_delete (other_vals, NULL);
	auto_array_delete (rvals, NULL);

	return rset;
}

set* set_union (set* s, set* other) {
	set* rvals = set_create (s->count + other->count, s->equals);
	if (rvals == NULL) {
		return NULL;
	}

	memcpy (rvals->data, s->data, sizeof (void*) * s->count);
	rvals->count = s->count;

	set_add_items (rvals, other->data, other->count);

	return rvals;
}

void set_delete (set* s, void (*delete_item)(void*)) {
	if (delete_item != NULL) {
		for (size_t i = 0; i < s->count; ++i) {
			delete_item (s->data[i]);
		}
	}	

	free (s->data);
	free (s);
	s = NULL;
}


