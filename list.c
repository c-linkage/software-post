/*

Copyright (c) 2020 Ethan D. Frolich

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <assert.h>
#include "selftest.h"
#include "mem.h"
#include "list.h"

struct link { int value; struct link *next; };
 
void list_init(struct list *list) 
{
	assert(list != NULL);

    list->next = NULL;
} 

void list_clear(struct list *list)
{
	struct link *link = NULL;

	assert(list != NULL);

	while(list->next != NULL)
	{
		link = list->next;
		list->next = link->next;
		mem_free(link);
	}
}

size_t list_count(struct list *list) 
{
	struct link *link = NULL;
	size_t count = 0;

	assert(list != NULL);

	link = list->next;

	while(link != NULL)
	{
		++count;
		link = link->next;
	}

	return count;
}

int list_contains(struct list *list, int value)
{
	struct link *link = NULL;

	assert(list != NULL);

	link = list->next;

	while(link)
	{
		if (link->value == value)
			return 1;

		link = link->next;
	}

	return 0;
}

int list_add(struct list *list, int value) 
{
	struct link *link = NULL;
	struct link **ptr = NULL;

	assert(list != NULL);

	link = mem_create(struct link);
	if (link == NULL) 
		return 0;

	link->value = value;
	link->next = NULL;
	ptr = &list->next;

	while ((*ptr) != NULL)
		ptr = &(*ptr)->next;

	(*ptr) = link;

	return 1;
}

void list_remove(struct list *list, int value)
{
	struct link *link = NULL;
	struct link **ptr = NULL;

	assert(list != NULL);

	for (ptr = &list->next; (*ptr) != NULL; ptr = &(*ptr)->next)
	{
		if ((*ptr)->value == value)
		{
			link = (*ptr);
			(*ptr) = link->next;
			mem_free(link);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////
//
// List system self-test
//
////////////////////////////////////////////////////////////////////////

SELF_TEST(list, SELF_TEST_LEVEL_DEFAULT)
{
	struct list s_list;
	struct list *list = &s_list;
	int rc = 0;

	// Initialize memory subsystem
	mem_init();

	// Verify list initialization
	list_init(list);
	SELF_TEST_ASSERT(list->next == NULL);
	SELF_TEST_ASSERT(list_count(list) == 0);

	// Verify that clearing an empty list works
	list_clear(list);
	SELF_TEST_ASSERT(list->next == NULL);
	SELF_TEST_ASSERT(list_count(list) == 0);

	// Add element to the list
	SELF_TEST_ASSERT(list_add(list, 100));
	SELF_TEST_ASSERT(list->next != NULL);
	SELF_TEST_ASSERT(list->next->value == 100);
	SELF_TEST_ASSERT(list_count(list) == 1);
	SELF_TEST_ASSERT(list_contains(list, 100));
	SELF_TEST_ASSERT(!list_contains(list, 200));

	// Verify that clearing a non-empty list works
	list_clear(list);
	SELF_TEST_ASSERT(list->next == NULL);
	SELF_TEST_ASSERT(list_count(list) == 0);

	// Add two elements to the list 
	SELF_TEST_ASSERT(list_add(list, 100));
	SELF_TEST_ASSERT(list_add(list, 200));
	SELF_TEST_ASSERT(list->next != NULL);
	SELF_TEST_ASSERT(list_count(list) == 2);
	SELF_TEST_ASSERT(list_contains(list, 100));
	SELF_TEST_ASSERT(list_contains(list, 200));

	// Verify that clearing a non-empty list works
	list_clear(list);
	SELF_TEST_ASSERT(list->next == NULL);
	SELF_TEST_ASSERT(list_count(list) == 0);

	// Rebuild a two element list and then remove the elements
	SELF_TEST_ASSERT(list_add(list, 100));
	SELF_TEST_ASSERT(list_add(list, 200));
	list_remove(list, 300);
	SELF_TEST_ASSERT(list_contains(list, 100));
	SELF_TEST_ASSERT(list_contains(list, 200));
	SELF_TEST_ASSERT(list_count(list) == 2);	
	list_remove(list, 100);
	SELF_TEST_ASSERT(!list_contains(list, 100));
	SELF_TEST_ASSERT(list_contains(list, 200));
	SELF_TEST_ASSERT(list_count(list) == 1);	
	list_remove(list, 200);
	SELF_TEST_ASSERT(!list_contains(list, 100));
	SELF_TEST_ASSERT(!list_contains(list, 200));
	SELF_TEST_ASSERT(list_count(list) == 0);	

	// NOTE: There was no specification on whether a list can have the same
	// element multiple times, so now we test what happens when we add the
	// same value twice with the assumption that this is allowed.

	SELF_TEST_ASSERT(list_add(list, 100));
	SELF_TEST_ASSERT(list_add(list, 100));
	SELF_TEST_ASSERT(list_count(list) == 2);
	SELF_TEST_ASSERT(list_contains(list, 100));
	list_remove(list, 100);
	SELF_TEST_ASSERT(list_count(list) == 1);
	SELF_TEST_ASSERT(list_contains(list, 100));
	list_remove(list, 100);
	SELF_TEST_ASSERT(list_count(list) == 0);
	SELF_TEST_ASSERT(!list_contains(list, 100));

	SELF_TEST_ASSERT(mem_uninit(NULL, NULL) == 1);

	rc = 1;

failure:
	return rc;
}
