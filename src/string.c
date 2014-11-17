
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

#include <stdlib.h>
#include <string.h>

auto_string* auto_string_create (size_t initial_size) {
	auto_string* s = malloc (sizeof (auto_string));

	if (s == NULL) {
		PERR ("malloc");
		return NULL;
	}

	s->buf = malloc (initial_size);
	if (s->buf == NULL) {
		PERR ("malloc");
		free (s);
		return NULL;
	}

	s->size = initial_size;
	s->count = 0;

	return s;
}

auto_string* auto_string_append (auto_string* s, char* cstr) {
	size_t n = strlen (cstr);

	if ((s->count + n) >= s->size) {
		size_t sz = 0;
		if (n < s->size) {
			sz = 2 * s->size;
		} else {
			sz = (2 * n) + s->size;
		}

		char* tmp = realloc (s->buf, sz);
		if (tmp == NULL) {
			PERR ("realloc");
			return NULL;
		}

		s->buf = tmp;
		s->size = sz;
	}

	if (s->count == 0) {
		strcpy (s->buf, cstr);
	} else {
		strcat (s->buf, cstr);
	}

	s->count += n;
	return s;
}

size_t auto_string_length (auto_string* s) {
	return strlen (s->buf);
}

void auto_string_delete (auto_string* s) {
	free (s->buf);
	free (s);
	s = NULL;
}

