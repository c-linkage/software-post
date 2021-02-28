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

#include "selftest.h"
#ifdef LINUX_SELFTEST_H
#include <stddef.h>
#include <stdio.h>

/*

There are three primary sections created under the GNU tool chain:

    .slftst.ini = arrays of function pointers to self tests
	.slftst.txt = executable code for all self tests
	.slftxst.str = constant strings used to report self test errors

The .slftst.ini section is further subdivided into twelve other sections,
ten of which contain arrays of function pointers and the remaining two are
"bookend" sections that bound the area of memory to be searched for
function pointers.

Running the self tests involves scanning the memory between the bookend
sections to search for non-NULL pointers to objects of type struct
self_test.  When such a pointer is found, the self-test name is printed and
the test is executed.

IMPORTANT: It is critical that linux_selftest.o be the first object module
passed to the linker for this to work.

*/

void __attribute__((__section__(".slftst.inis"))) *self_test_list_start = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_1))) *self_test_list_1 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_2))) *self_test_list_2 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_3))) *self_test_list_3 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_4))) *self_test_list_4 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_5))) *self_test_list_5 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_6))) *self_test_list_6 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_7))) *self_test_list_7 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_8))) *self_test_list_8 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_9))) *self_test_list_9 = NULL;
void __attribute__((__section__(SELF_TEST_LEVEL_10))) *self_test_list_10 = NULL;
void __attribute__((__section__(".slftst.inie"))) *self_test_list_end = NULL;

static const char SELF_TEST_RO msg_naked[] = "%s\n";
static const char SELF_TEST_RO msg_decorated[] = "%s:%zu: %s\n";
static const char SELF_TEST_RO msg_linker_warning[]  =
	"self-test: warning: no self tests executed; is linux_selftest.o the first file passed to the linker?";

void sys_self_test_report(const char *msg, const char *file, size_t line)
{
	if (file == NULL)
		fprintf(stderr, msg_naked, msg);
	else
		fprintf(stderr, msg_decorated, file, line, msg);
}

int sys_self_test_run(self_test_report_pf report, unsigned flags)
{
	const struct self_test **test;
	size_t test_count;
	int rc;

	rc = 1;
	test_count = 0;
	test = (const struct self_test **)&self_test_list_start;

	while (++test < (const struct self_test **)&self_test_list_end)
	{
		if ((*test) != NULL)
		{
			++test_count;

			if ((*test)->name != NULL) 
				report((*test)->name, NULL, 0);
			
			if (!(*test)->func(report)) 
			{
				if (flags & SELF_TEST_FLAG_STOP_ON_FAILURE) 
					return 0;
				rc = 0;
			}
		}
	}

	if (test_count == 0)
		report(msg_linker_warning, NULL, 0);

	return rc;
};

#endif /* LINUX_SELFTEST_H */
