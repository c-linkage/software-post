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

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"
#include "selftest.h"

// 
// This union will create a data type that has the strictest alignment
// requirements of all data types.
//

union align
{
	char	c;
	int		i;
	float	f;
	double	d;
	void 	*p;
};

// 
// The heap marker is a doubly-linked list of all memory allocations with
// an attached file and line for each allocation.  
//
// The structure is technically variably sized as the 'data' portion of the
// structure will hold the actual data requested by the calling
// application. The alignment requirements of the union will ensure that
// the pointer returned to the caller will be properly aligned.
//

struct marker
{
	struct marker	*prev;
	struct marker	*next;
	const char		*file;
	int				line;
	union align		data;
};

// 
// Statically declare the head of the doubly-linked list.

static struct marker s_marker_list;

//
// Initialize the memory subsystem.
//
// The head of the doubly-linked list will be properly configured and the
// file and line will be set to indicate no association with an actual
// allocation.
//
void mem_init(void)
{
	struct marker *list = &s_marker_list;
	
	list->prev = list;
	list->next = list;
	list->file = NULL;
	list->line = 0;
}

//
// Uninitialize the memory subsystem an optionally report memory leaks.
//
// If the reporting function is specified, walk the heap markers and call
// the reporting function for each marker remaining. 
//
// Return code 1 means success, 0 means memory was leaked.
//
int mem_uninit(mem_report_pf report, void *data)
{
	int rc = 1;

	if (s_marker_list.next != &s_marker_list)
	{
		rc = 0;

		if (report != NULL)
		{
			struct marker *marker = s_marker_list.next;

			do 
			{
				report(marker->file, marker->line, data);
				marker = marker->next;
			}
			while (marker != &s_marker_list);
		}
	}

	memset(&s_marker_list, 0, sizeof(s_marker_list));

	return rc;
}

//
// Allocate memory and record the calling location.
//
// A memory region will be allocated with space for both the client
// requested are and the marker. Once the marker has been configured and
// linked into the list of memory allocations, return the address of the
// client's portion of the memory block.
//
void *mem_alloc_internal(size_t size, const char *file, int line)
{
	struct marker	*list = &s_marker_list;
	struct marker	*marker;
	size_t 			marker_size;
	intptr_t		addr;

	if (s_marker_list.next == NULL)
		return NULL; // Memory subsytem not initialized

	// XXX: There is a possibility of size overflow, but I'm not going to
	// test for that because this is an example.

	marker_size = offsetof(struct marker, data) + size;
	marker = (struct marker *)malloc(marker_size);

	if (marker == NULL)
		return NULL;

	marker->file = file;
	marker->line = line;

	// XXX: This is not thread safe because it is an example.

	marker->next = list->next;
	marker->prev = list;
	marker->next->prev = marker;
	marker->prev->next = marker;

	addr = (intptr_t)marker;
	addr += offsetof(struct marker, data);
	
	return (void *)addr;
}

//
// Free a memory region.
//
// The marker's address will be computed from the client's address and then
// the marker will be removed from the doubly-linked list. After removal,
// the block will then be freed by the standard block allocator.
//
void mem_free_internal(void *ptr)
{
	struct marker	*marker;
	intptr_t		addr;

	if (ptr == NULL)
		return;

	addr = (intptr_t)ptr;
	addr -= offsetof(struct marker, data);
	marker = (struct marker *)addr;

	// XXX: This is not thread safe because it is an example.

	if (s_marker_list.next != NULL)
	{
		// Only remove the item from the list if it looks like the marker
		// list is initialized.  Skipping this when uninitialized avoid
		// access violations but still allows memory to be freed.

		marker->next->prev = marker->prev;
		marker->prev->next = marker->next;
		marker->next = NULL;
		marker->prev = NULL;
	}

	free(marker);
}

////////////////////////////////////////////////////////////////////////
//
// Memory Subsystem Self Test
//
////////////////////////////////////////////////////////////////////////

struct self_test_data
{
	self_test_report_pf report;
	int					line;
	int					leak_count;
};

static void SELF_TEST_FUNC mem_report_self_test(
	const char *file, int line, void *data)
{
	static const char SELF_TEST_RO msg_leak[] =
		"info: self-test leak detected";
	
	struct self_test_data	*self_test_data = (struct self_test_data *)data;

	self_test_data->line = line;
	self_test_data->leak_count += 1;

	self_test_data->report(msg_leak, file, line);
}

SELF_TEST(memory, SELF_TEST_LEVEL_1)
{
	struct self_test_data data;
	int *p1, *p2;
	int line;
	int	rc = 0;

	data.report = self_test_report; // Use the self-test reporting function
	data.leak_count = 0;
	data.line = 0;

	// Do the basic check to make sure allocation and deallocation work

	mem_init();
	p1 = mem_create(int);
	SELF_TEST_ASSERT(p1 != NULL);
	mem_free(p1);
	mem_uninit(mem_report_self_test, &data);
	SELF_TEST_ASSERT(data.leak_count == 0);
	SELF_TEST_ASSERT(data.line == 0);

	// Test leaking one allocation

	data.leak_count = 0;
	data.line = 0;

	mem_init();
	p1 = mem_create(int); line = __LINE__;
	SELF_TEST_ASSERT(p1 != NULL);
	mem_uninit(mem_report_self_test, &data);
	SELF_TEST_ASSERT(data.leak_count == 1);
	SELF_TEST_ASSERT(data.line == line);
	mem_free(p1);

	// Test leaking two allocations

	data.leak_count = 0;
	data.line = 0;

	mem_init();
	p1 = mem_create(int); line = __LINE__;
	p2 = mem_create(int); line = __LINE__;
	SELF_TEST_ASSERT(p1 != NULL);
	SELF_TEST_ASSERT(p2 != NULL);
	mem_uninit(mem_report_self_test, &data);
	SELF_TEST_ASSERT(data.leak_count == 2);
	mem_free(p1);
	mem_free(p2);

	// Test leaking first allocation

	data.leak_count = 0;
	data.line = 0;

	mem_init();
	p1 = mem_create(int); line = __LINE__;
	p2 = mem_create(int);
	SELF_TEST_ASSERT(p1 != NULL);
	SELF_TEST_ASSERT(p2 != NULL);
	mem_free(p2);
	mem_uninit(mem_report_self_test, &data);
	SELF_TEST_ASSERT(data.leak_count == 1);
	SELF_TEST_ASSERT(data.line == line);
	mem_free(p1);

	// Test leaking second allocation

	data.leak_count = 0;
	data.line = 0;

	mem_init();
	p1 = mem_create(int); 
	p2 = mem_create(int); line = __LINE__;
	SELF_TEST_ASSERT(p1 != NULL);
	SELF_TEST_ASSERT(p2 != NULL);
	mem_free(p1);
	mem_uninit(mem_report_self_test, &data);
	SELF_TEST_ASSERT(data.leak_count == 1);
	SELF_TEST_ASSERT(data.line == line);
	mem_free(p2);

	rc = 1;

failure:
	mem_uninit(NULL, NULL);
	return rc;
}
