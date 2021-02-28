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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "selftest.h"
#include "mem.h"
#include "list.h"

// strcasecmp() is a recent addition to the C standard, and many versions
// of the Microsoft C tool chain do not support it.  Wrap the string
// comparison functions to make this easier for me.

#if defined(_MSC_VER)
#define streq(a,b) (0 == strcmp((a),(b)))
#define strieq(a,b) (0 == _stricmp((a),(b)))
#else
#include <strings.h>
#define streq(a,b) (0 == strcmp((a),(b)))
#define strieq(a,b) (0 == strcasecmp((a),(b)))
#endif

static int f_self_test = 0;

void parse_args(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i)
	{
		if (streq(argv[i], "--self-test"))
			f_self_test = 1;
		if (strieq(argv[i], "/SELF-TEST"))
			f_self_test = 1;
	}
}

void mem_leak_detected(const char *file, int line, void *data)
{
	fprintf(stderr, "%s:%d: error: memory leak detected!\n",
		file, line);
}

int main(int argc, char **argv)
{
	struct list *list;
	int max_tries = 10;
	int	guess;

	parse_args(argc, argv);

	if (f_self_test)
	{
		// Report on stderr under LINUX and OutputDebugString() under Windows

		if (!self_test_run(SELF_TEST_SYSTEM_REPORT, SELF_TEST_FLAG_NONE))
			return 0;
	}

	mem_init();
	list = mem_create(struct list);
	list_init(list);

	for (int i = 0; i < 10; ++i)
	{
		float normalized = (float)rand() / (float)RAND_MAX;
		int   scaled = (int)(normalized * 25.f);

		list_add(list, scaled);
	}

	for (int i = 0; i < max_tries; ++i)
	{
		printf("You have %d tries to pick one of my numbers.\n", max_tries - i);
		printf("What number do you guess?\n");
		scanf("%d", &guess);
		
		if (list_contains(list, guess))
		{
			printf("You guessed right!\n");
			break;
		}
		else
		{
			printf("You guess wrong!\n");
		}
	}

	list_clear(list);
	mem_free(list);

	mem_uninit(mem_leak_detected, NULL);

	return 0;
}
