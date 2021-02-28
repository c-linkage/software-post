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
#ifdef WIN32_SELFTEST_H
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <crtdbg.h>

/*

There are three primary sections created under the Microsoft tool chain:
	
	slftsti = array of pointers to self-test descriptors
	slftstt = executable code for all self tests
	slftstr = constants (e.g. strings) used to report self test errors

The slftsti section is subdivided into twelve subsections, ten of which
contain the arrays of functions pointers for self tests; the remaining
to are "bookend" sections that bound the area of memory to be searched for
descriptor pointers.

The Microsoft tool chain allows section names with '$' in them. The part of
the identifier before the '$' is the final name of the section, and the
part after the '$' is used to order the contents of that section
alphabetically.  For example, if there are two sections called "test$a" and
"test$b" then the linker will create one section called "test" with the
contents of "test$a" placed before the contents of "test$b" in the section.

The challenge with the Microsoft tool chain is that there are no
padding guarantees when the section is finalized. That means that there
could be a gap of arbitrary size in section "test" between the contents of
"test$a" and "test$b". What *is* guaranteed is that the padding will
always be zero.

Running the self tests involves scanning the memory between the bookend
sections "slftsti$a" and "slftsti$z" to look for non-NULL pointers to
objects of type struct self_test.  When such a pointer is found, the
self-test name is printed and the test is excuted.

*/

#pragma section("slftsti$a", read, shared)
__declspec(allocate("slftsti$a")) 
	const struct self_test *win32_self_test_start = (struct self_test *)1;

#pragma section("slftsti$z", read, shared)
__declspec(allocate("slftsti$z")) 
	const struct self_test *win32_self_test_end = (struct self_test *)1;

void sys_self_test_report(
	const char *message, const char *file, size_t line)
{
	char buffer[300];

	if (file == NULL)
		snprintf(buffer, sizeof(buffer), "%s\n", message);
	else
		snprintf(buffer, sizeof(buffer), "%s(%u): %s\n", file, line, message);

	buffer[sizeof(buffer)-1] = '\0';

	OutputDebugStringA(buffer);
}

int sys_self_test_run(self_test_report_pf report, unsigned flags)
{
	const struct self_test **test;
	int rc;

	test = &win32_self_test_start;
	rc = 1;

	while(++test < &win32_self_test_end)
	{
		if ((*test) != NULL)
		{
			if ((*test)->name != NULL) 
				report((*test)->name, NULL, 0);

			if (!(*test)->func(report)) 
			{
				if (flags & SELF_TEST_FLAG_STOP_ON_FAILURE) 
					return 0;
				rc = 0;
			}

			// Call this function to ensure self-tests didn't leak.
			// Leaks during the self-test will cause false-positive 
			// memory leaks later during run-time

			_CrtDumpMemoryLeaks();
		}
	}

	return rc;
}
#endif /* WIN32_SELFTEST_H */
