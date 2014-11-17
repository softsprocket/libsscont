/** @file */
/**
* \mainpage 
* 	libsscont - a library of software containers
*    ==============================================================
* \code{.unparsed}   
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
* \endcode
*/

#ifndef CONTAINER_H_
#define CONTAINER_H_

#ifndef STDINT_HEADER_INCLUDED_
#define STDINT_HEADER_INCLUDED_
#include <stdint.h>
#endif // STDINT_HEADER_INCLUDED_

#ifndef STDLIB_HEADER_INCLUDED_
#define STDLIB_HEADER_INCLUDED_
#include <stdlib.h>
#endif // STDLIB_HEADER_INCLUDED_

#ifndef SYS_TYPES_HEADER_INCLUDED_
#define SYS_TYPES_HEADER_INCLUDED_
#include <sys/types.h>
#endif // SYS_TYPES_HEADER_INCLUDED_


/************************************************************************
 * 				auto_array
 */				

/**  A generic data store for pointers that auto resizes.
 *  @see auto_array_create.
 */
typedef struct {
	size_t size;  /**< current allocated size */
	size_t count; /**< current number of stored items */
	void** data;  /**< data table */
} auto_array;

/**
 * Initializes a pointer to an auto_array structure.
 * @param initial_size initializes the size of the storage buffer. 
 * @return an auto_array pointer or NULL if an error occurs.
 */
auto_array* auto_array_create (size_t initial_size);

/**
 * Stores a pointer immediately after the last stored item. 
 * @param aa the auto_array to use for storage
 * @param data the pointer to store
 * @return the current count or -1 if an error occurs.
 */
ssize_t auto_array_add (auto_array* aa, void* data);

/**
 * Stores a pointer at a specific location. 
 * @param aa the auto_array to use for storage
 * @param pos the location to store the pointer 
 * 	must be <= the count member of auto_array. If pos is equal to count it is the 
 * 	equivalent to auto_array_add, otherwise the pointer previously at the position
 * 	is replaced. Note that memory from the replace pointer is not reclaimed. 
 * @param data a pointer to be stored
 * @return the current count or -1 in case of a failure.
 */
ssize_t auto_array_put (auto_array* aa, size_t pos, void* data);

/**
 * Returns the pointer stored at the specified position.
 * @param aa the auto_array to retrieve from
 * @param pos the position of the pointer being requested
 * @return an auto_array pointer or NULL if an error occurs 
 */
void* auto_array_get (auto_array* aa, size_t pos);

/**
 * Returns the pointer stored at the last position.
 * @param aa the auto_array to retrieve from
 * @return an auto_array pointer or NULL if an error occurs 
 */
void* auto_array_last (auto_array* aa);

/**
 * Inserts a pointer at a specific location. Moves the following pointers up one position. 
 * @param aa the auto_array to use for storage
 * @param pos the location to store the pointer 
 * 	If pos is equal to count it is the equivalent to auto_array_add.
 * @param data a pointer to be stored
 * @return the current count or -1 in case of a failure.
 */
ssize_t auto_array_insert (auto_array* aa, size_t pos, void* data);

/**
 * Removes the pointer at pos and shifts the rest of the 
 * store down.
 * @param aa the auto_array to use for storage
 * @param pos the location to remove 
 * @return the removed pointer or NULL in caseof failure 
 */
void* auto_array_remove (auto_array* aa, size_t pos);

/**
 * Frees allocated memory.
 * @param aa the auto_array to free
 * @param delete_entry a function pointer that will be called for each stored pointer.
 * 	It can be used to free memory. It may be NULL in which case memory must be 
 * 	recovered by the programmer.
 */
void auto_array_delete (auto_array* aa, void (*delete_entry)(void* entry));

/***************************************************************************************
 * 				hash_table
*/

/**
 * key value storage allocated internally
 */ 
typedef struct {
	uint32_t hash; /**< storage for the hash */
	char* key;    /**< storage for the key */
	void* value; /**<  storage for the value */
} hash_entry;

/**
 * used internally
 */
typedef struct {
	size_t size;         /**< current allocated bucket size */
	size_t count;        /**< current item count */
	hash_entry** entries; /**< entry store */
} hash_bucket;

/**
 * A key/value store for generic pointers.
 * @see hash_table_create
 */
typedef struct {
	size_t size;          /**< the number of buckets */
	hash_bucket** buckets; /**< bucket store */
} hash_table;

/**
 * Initializes and returns a pointer to a hash_table object.
 * @param size_table the number of buckets that will be initialized. Each bucket will 
 * 	auto size. A larger table will reduce collisions.
 * @return a pointer to a hash_table 
 */
hash_table* hash_table_create (size_t size_table);

/**
 * Stores a pointer, using a hash of the key (Paul Hsieh's fast hash is used).
 * @param ht the hash_tale to use for storage
 * @param key the key to hash
 * @param value the pointer to store
 * @return a pointer to a hash_entry object of NULL if an error occurs
 */
hash_entry* hash_table_put (hash_table* ht, char* key, void* value);

/**
 * Returns the first entry that matches the key.
 * @param ht the hash_table to search
 * @param key the key to search on
 * @return the pointer stored as the value associated with the key or NULL if an error occurs
 */
void* hash_table_get (hash_table* ht, char* key);

/**
 * Returns the all entries that match the key.
 * @param ht the hash_table to search
 * @param key the key to search on
 * @return an auto_array pointer containing the values associated with the key or NULL if an error occurs
 */
auto_array* hash_table_get_all (hash_table* ht, char* key);

/**
 * Removes the first entry that matches the key.
 * @param ht the hash_table to remove it from
 * @param key the key that identifies the entry
 * @return the pointer that was stored as a value or NULL in event of an error 
 */
void* hash_table_remove (hash_table* ht, char* key);

/**
 * Returns an auto_array of all the keys in the hash_table.
 * @param ht the hash_table containing the keys
 * @return a pointer to an auto_array or NULL if an error occurs
 */
auto_array* hash_table_keys (hash_table* ht);

/**
 * Frees memory allocated for the hash_table. 
 * @param ht the hash_table to free
 * @param delete_value a function pointer that will be called for each stored pointer.
 * 	It can be used to free memory. It may be NULL in which case memory must be 
 * 	reclaimed by the programmer.
 */
void hash_table_delete (hash_table* ht, void (*delete_value)(void*));



/** 					
 * A fixed size container that models set operations in math. 
 * @see set_create
 */
typedef struct {
	size_t size;  /**< The number of items the set can contain */
	size_t count; /**< The number of items in the set */
	void** data;  /**< data table */
	int (*equals) (void*, void*); /**< function pointer used to establish equality */
} set;


/**
 * Initializes and returns set. 
 * @param size the number of elements the set will hold
 * @param equals a pointer to a function to be used to test item equality.
 * @return a pointer to a set
 */
set* set_create (size_t size, int (*equals) (void*, void*));

/**
 * Get the index of the item equal to the item in the params.
 * @param s the set to search
 * @param item to search for (equivelence is determined by the 
 * 	function pointer passed in set_create
 * @return the index of the item or -1 if an error occurs.
 */
ssize_t set_get_item_index (set* s, void* item);

/** 
 * Adds a pointer to the set.
 * @param s the set to add the pointer to 
 * @param item the pointers to add
 * @return the current count or -1 if an error has occurred
 */
ssize_t set_add_item (set* s, void* item);

/** 
 * Adds a collection of pointers to the set.
 * @param s the set to add the pointers to 
 * @param items an array of pointers to add
 * @param size the number of pointers
 * @return the current count or -1 if an error has occurred
 */
ssize_t set_add_items (set* s, void** items, size_t size);

/**
 * Gets the pointer stored at certain position.
 * @param s the set to get the pointer from
 * @param pos the position to return
 * @return the pointer stored at that position
 */
void* set_get_item (set* s, size_t pos);

/**
 * Returns the power set of a set.
 * @param s the set to use to form the power set
 * @return a pointer to a set containing all of the subsets of the set 
 * 	or NULL in the case of an error.
 * 	Memory should be reclaimed, when the returned set is no longer
 * 	used, by calling set_delete with a  second param of a function
 * 	that deletes the subsets i.e.
 * 	\code{.c}
 *	void free_set (void* v) {
 *		set_delete (v, NULL);
 *	}
 *	\endcode
 *
 */
set* set_all_subsets (set* s);

/**
 * Returns the union of two sets using the equals function 
 * in the first set.
 * @param s one of the sets to join
 * @param other on of the sets to join
 * @return a pointer to a set containing the union of the 
 * 	two sets of NULL in the case of an error.
 * 	Memory should be reclaimed, when the returned set is no longer
 * 	used, by calling set_delete with a NULL second param 
 */
set* set_union (set* s, set* other);

/**
 * Returns the intersection of two sets using the equals function 
 * in the first set.
 * @param s one of the sets to intersect
 * @param other on of the sets to intersect
 * @return a pointer to a set containing the intersection of the 
 * 	two sets of NULL in the case of an error.
 * 	Memory should be reclaimed, when the returned set is no longer
 * 	used, by calling set_delete with a NULL second param 
 */
set* set_intersection (set* s, set* other);

/**
 * Frees memory for the set.
 * @param s the set to free
 * @param delete_item a function pointer that will be called for each stored pointer.
 * 	It can be used to free memory. It may be NULL in which case memory must be 
 * 	reclaimed by the programmer.
 */
void set_delete (set* s, void (*delete_item)(void*));


/** 					
 * An auto sizing char buffer.
 * @see auto_string_create
 */
typedef struct {
	size_t size; /**< current allocated size */
	size_t count; /**< current count of used elements */
	char* buf; /**< Nul terminated char buffer */
} auto_string;

/**
 * Initialize and return a pointer to an auto_string.
 * @param initial_size the initial size of the buffer
 * @returns a pointer to an auto_string
 */
auto_string* auto_string_create (size_t initial_size);

/**
 * Appends a nul terminated string to the buffer.
 * @param str the auto_string pointer
 * @param cstr the nul terminated strng to append to the string
 * @return the auto_string or NULL if there is an error
 */
auto_string* auto_string_append (auto_string* str, char* cstr);

/**
 * Returns the length of the buffer. The same as strlen (str->buf).
 * @param str the auto_string pointer
 * @return the length of the string
 */
size_t auto_string_length (auto_string* str);

/**
 * 
 * Free the auto_string and buffer.
 * @param str the auto_string to free
 */
void auto_string_delete (auto_string* str);


#endif // CONTAINER_H_


