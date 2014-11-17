

    libsscont - a library of software containers

    Copyright (C) 2014 Gregory Ralph Martin
    greg at softsprocket dot com

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA



A library of table based containers for pointers.

hash_table, auto_array and auto_string automatically grow, set is a fixed size container.

Requires a POSIX compliant make and C compiler that is gcc compatible using std=c11.

See INSTALL for the make targets.

See tests/main.c for usage examples.

AUTO_ARRAY

SYNOPSIS

       #include <softsprocket/containers.h>

       auto_array* auto_array_create (size_t initial_size);
       ssize_t auto_array_add (auto_array* aa, void* data);
       ssize_t auto_array_put (auto_array* aa, size_t pos, void* data);
       void* auto_array_get (auto_array* aa, size_t pos);
       void* auto_array_last (auto_array* aa);
       ssize_t auto_array_insert (auto_array* aa, size_t pos, void* data);
       void* auto_array_remove (auto_array* aa, size_t pos);
       void auto_array_delete (auto_array* aa, void (*delete_entry)(void* entry));

       Link with -lsscont.

DESCRIPTION

       These functions provide an interface to an auto sizing array in C.

       auto_array* auto_array_create (size_t initial_size)
           initial_size - the number of entries the array is initialized for.
           returns - a pointer to an auto_array or NULL if an error occurs

       void auto_array_delete (auto_array* aa, void (*delete_entry)(void* entry))
           aa - the auto_array to operate on
           delete_entry - if not NULL this function will be csalled on each member of the array before the array is freed.

       The structure that is returned by auto_array_create:

       typedef struct {
            size_t size;   // current allocated size
            size_t count;  // number of elements
            void** data;
       } auto_array;

       ssize_t auto_array_add (auto_array* aa, void* data)
           aa - the auto_array to operate on
           data - the pointer to be stored
           returns - auto_array count or -1 if an error occurs

       ssize_t auto_array_put (auto_array* aa, size_t pos, void* data)
           aa - the auto_array to operate on
           pos - the pos to store the item - replacing the item at that location. It is the programmers responsibility to recover the memory allocated for the pointer that was stored there.
           data - the item to be stored
           returns - the number of items in the array or -1 if an error occurs

       void* auto_array_get (auto_array* aa, size_t pos)
           aa - the auto_array to operate on
           pos - the index to the item to retrieve
           returns - an item or NULL if no item at that index

       void* auto_array_last (auto_array* aa)
           aa - the auto_array to operate on
           returns - the item at the end of the array or NULL if the array is empty

       ssize_t auto_array_insert (auto_array* aa, size_t pos, void* data)
           aa - the auto_array to operate on
           pos - the pos to insert the item - the items that follow are shifted
           data - the item to be stored
           returns - the number of items in the array or -1 if an error occurs

       void* auto_array_remove (auto_array* aa, size_t pos);
           aa - the auto_array to operate on
           pos - the index of the item to be removed - following items are shifted down
           returns - the item that has been removed or NULL if an error has occurred

HASH_TABLE

SYNOPSIS

       #include <softsprocket/containers.h>

       hash_table* hash_table_create (size_t size_table)
       hash_entry* hash_table_put (hash_table* ht, char* key, void* value);
       void* hash_table_get (hash_table* ht, char* key);
       auto_array* hash_table_get_all (hash_table* ht, char* key);
       void* hash_table_remove (hash_table* ht, char* key);
       auto_array* hash_table_keys (hash_table* ht);
       void hash_table_delete (hash_table* ht, void (*delete_value)(void*));

       Link with -lsscont.

DESCRIPTION

       These functions provide an interface to an auto sizing hash_table in C.

       hash_table* hash_table_create (size_t size_table)
           size_table param - the number of buckets the hash_table will have.
           returns - a pointer to a hash_table or NULL if an error occurs

       void hash_table_delete (hash_table* ht, void (*delete_value)(void*))
           ht - the hashtable to operate on
           delete_value - a function called on each item before the hash_table is freed. It may be NULL.

       The structure that is returned by hash_table_create:
       typedef struct {
            size_t size;
            hash_bucket** buckets;
       } hash_table;

       The buckets:
       typedef struct {
            size_t size;
            size_t count;
            hash_entry** entries;
       } hash_bucket;

       The entries:
       typedef struct {
            uint32_t hash;
            char* key;
            void* value;
       } hash_entry;

       hash_entry* hash_table_put (hash_table* ht, char* key, void* value)
           ht - the hashtable to operate on
           key - a string key that will be used to identify the entry. A copy of the key is made and will be freed when hash_table_delete is called. Freeing any memory from the original is the programmers responsibility.
           value - the item to be stored.
           returns - the hash_entry object that is used internally or NULL in the event of an error.

       void* hash_table_get (hash_table* ht, char* key)
           ht - the hashtable to operate on
           key - the key to the entry to retrieve
           returns - the first stored value found with that key or NULL if not found

       auto_array* hash_table_get_all (hash_table* ht, char* key)
           ht - the hashtable to operate on
           key - the key to the entries to retrieve
           returns - all entries with that key or NULL if none are found

       void* hash_table_remove (hash_table* ht, char* key);
           ht - the hashtable to operate on
           key - the key to the entry to remove
           returns - the first stored value found with that key, removing it from the table, or NULL if not found

       auto_array* hash_table_keys (hash_table* ht)
           ht - the hashtable to operate on
           returns - an auto_array containing all the keys in the table

SET

SYNOPSIS

       #include <softsprocket/containers.h>

       set* set_create (size_t size, int (*equals) (void*, void*));
       ssize_t set_get_item_index (set* s, void* item);
       void* set_get_item (set* s, size_t pos);
       ssize_t set_add_item (set* s, void* item);
       ssize_t set_add_items (set* s, void** items, size_t size);
       set* set_all_subsets (set* s);
       set* set_union (set* s, set* other);
       set* set_intersection (set* s, set* other);
       void set_delete (set* s, void (*delete_item)(void*));

       Link with -lsscont.

DESCRIPTION

       These functions provide an interface to a fixed size set container and set operations.

       set* set_create (size_t size, int (*equals) (void*, void*))
           size param - the number of pointers it will store.
           equals - a function that will be used to determine equality for the set. The set contains unique items based on this equality.
           returns - a pointer to a set or NULL if an error occurs

       void set_delete (set* s, void (*delete_item)(void*))
           s - the set to operate on
           delete_item - a function to be called on each item. If NULL the programmer must free item memory

       The structure that is returned by set_create:
       typedef struct {
            size_t size;             // total space available
            size_t count;            // current count
            void** data;             // pointer storage
            int (*equals) (void*, void*); // saved equality function
       } set;

       ssize_t set_get_item_index (set* s, void* item)
           s - a pointer to the set to be operated on
           item - the item whose index will be returned
           returns - the index of an item or -1 if none is found.

       ssize_t set_add_item (set* s, void* item)
           s - a pointer to the set to be operated on
           item - the item to add
           returns - the current count or -1 if an error occurs

       ssize_t set_add_items (set* s, void** items, size_t size)
           s - a pointer to the set to be operated on
           items - the items to be added
           size - thenumber of items in the array
           returns - the current count or -1 if an error occurs

       void* set_get_item (set* s, size_t pos)
           s - a pointer to the set to be operated on
           pos - the index of the item
           returns - the item at the index or NULL if an error occurs

       set* set_all_subsets (set* s)
           s - a pointer to the set to be operated on
           returns  -  a  set  of all the sets in the set passed as the param or NULL if there's an error.  Memory for the power set should be reclaimed by calling set_delete with a function pointer that calls set_delete
           with a NULL function pointer.  i.e.
           void set_deleter (void* v) {
                set_delete (v, NULL);
           }

       set* set_union (set* s, set* other);
           s - a set to perform union on
           other - a set to perform union on
           returns - a set of the union of the two sets. Memory should be reclaimed on the return set by calling set_delete with a NULL function pointer

       set* set_intersection (set* s, set* other);
           s - a set to perform intersection on
           other - a set to perform intersection on
           returns - a set of the intersection of the two sets. Memory should be reclaimed on the return set by calling set_delete with a NULL function pointer

AUTO_STRING

SYNOPSIS

       #include <softsprocket/containers.h>

       auto_string* auto_string_create (size_t initial_size);
       auto_string* auto_string_append (auto_string* as, char* str);
       size_t auto_string_length (auto_string* as);
       void auto_string_delete (auto_string* as);

       Link with -lsscont.

DESCRIPTION

       These functions provide an interface to an auto sizing string in C.

       auto_string* auto_string_create (size_t initial_size)
           initial_size - the number of char the string is initialized for.
           returns - a pointer to an auto_string or NULL if an error occurs

       void auto_string_delete (auto_string* as)
           as - the auto_string to operate on - frees allocated memory

       The structure that is returned by auto_string_create:

       typedef struct {
            size_t size; /** current allocated size */
            size_t count; /** current count of used elements */
            char* buf; /** Nul terminated char buffer */
       } auto_string;

       The member, buf, is a C string.

       void* auto_string_append (auto_string* as, char* str)
           as - the auto_string to operate on
           str - the string to append - this function makes acopy of the string. The original string is not changed and the programmer must reclaim any memory that has been allocated for it.
           returns - an auto_string or NULL if an error occurs

       size_t auto_string_length (auto_string* as)
           as - the auto_string to operate on
           returns - the current length of buf (strlen)


