
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

#include <stdio.h>
 
auto_array* auto_array_create (size_t initial_size) {
	auto_array* aa = malloc (sizeof (auto_array));

	if (aa == NULL) {
		PERR ("malloc");
		return NULL;
	}

	aa->data = malloc (sizeof (void*) * initial_size);

	if (aa->data == NULL) {
		PERR ("malloc");
		free (aa);
		return NULL;
	}

	aa->size = initial_size;
	aa->count = 0;

	return aa;
}

ssize_t auto_array_add (auto_array* aa, void* data) {
	return auto_array_put (aa, aa->count, data);
}

ssize_t auto_array_put (auto_array* aa, size_t pos, void* data) {
	if (pos > aa->count) {
		return 0;
	}

	if (pos == (aa->size)) {
		size_t s = aa->size * 2;
		void* tmp;
		if ((tmp = realloc (aa->data, s * sizeof (void*))) == NULL) {
			PERR ("realloc");
			return 0;
		}
		aa->data = tmp;
		aa->size = s;
	}

	aa->data[pos] = data;

	if (pos == aa->count) {
		aa->count++;
	}

	return aa->count;
}

void* auto_array_last (auto_array* aa) {
	if (aa->count == 0) {
		return NULL;
	}

	return auto_array_get (aa, aa->count - 1);
}

void* auto_array_get (auto_array* aa, size_t pos) {
	if (pos >= aa->count) {
		return NULL;
	}

	return aa->data[pos];
}

void* auto_array_remove (auto_array* aa, size_t pos) {
	if (pos >= aa->count) {
		return NULL;
	}
	
	void* rv = aa->data[pos];
	
	for (size_t i = (pos + 1); i < aa->count; ++i) {
		aa->data[i - 1] = aa->data[i];
	}
	
	aa->count--;

	return rv;
}

ssize_t auto_array_insert (auto_array* aa, size_t pos, void* data) {
	if (pos >= aa->count) {
		return -1;
	}

	if ((aa->count + 1) == aa->size) {
		void* tmp;
		size_t s = aa->size * 2;
		if ((tmp = realloc (aa->data, s * sizeof (void*))) == NULL) {
			PERR ("realloc");
			return -1;
		}
		aa->data = tmp;
		aa->size = s;
	}

	
	for (size_t i = aa->count; i >= pos; --i) {
		aa->data[i + 1] = aa->data[i];
	}
	
	aa->data[pos] = data;

	aa->count++;

	return aa->count;
}

void auto_array_delete (auto_array* aa, void (*delete_entry)(void* entry)) {
	if (delete_entry != NULL)  {
		for (size_t i = aa->count; i > 0; --i) {
			delete_entry (aa->data[i - 1]);
		}
	}

	free (aa->data);
	free (aa);
	aa = NULL;
}

