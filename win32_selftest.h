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
#ifndef WIN32_SELFTEST_H
#define WIN32_SELFTEST_H

// Alway use C calling convention

#define SELF_TEST_DECL __cdecl

// Define the names of the required sections for each test level

#define SELF_TEST_LEVEL_1  "slftsti$b"
#define SELF_TEST_LEVEL_2  "slftsti$e"
#define SELF_TEST_LEVEL_3  "slftsti$i"
#define SELF_TEST_LEVEL_4  "slftsti$k"
#define SELF_TEST_LEVEL_5  "slftsti$m"
#define SELF_TEST_LEVEL_6  "slftsti$o"
#define SELF_TEST_LEVEL_7  "slftsti$q"
#define SELF_TEST_LEVEL_8  "slftsti$s"
#define SELF_TEST_LEVEL_9  "slftsti$u"
#define SELF_TEST_LEVEL_10 "slftsti$w"

// Set the default test level

#define SELF_TEST_LEVEL_DEFAULT SELF_TEST_LEVEL_5

// Forward declare all sections; unused sections are removed by the linker

#pragma section(SELF_TEST_LEVEL_1, read, shared)
#pragma section(SELF_TEST_LEVEL_2, read, shared)
#pragma section(SELF_TEST_LEVEL_3, read, shared)
#pragma section(SELF_TEST_LEVEL_4, read, shared)
#pragma section(SELF_TEST_LEVEL_5, read, shared)
#pragma section(SELF_TEST_LEVEL_6, read, shared)
#pragma section(SELF_TEST_LEVEL_7, read, shared)
#pragma section(SELF_TEST_LEVEL_8, read, shared)
#pragma section(SELF_TEST_LEVEL_9, read, shared)
#pragma section(SELF_TEST_LEVEL_10, read, shared)

#pragma section("slftstr", read, shared)

// Define platform-specific macros

#define SELF_TEST_FUNC __declspec(code_seg("slftstt")) SELF_TEST_DECL
#define SELF_TEST_RO __declspec(allocate("slftstr"))
#define SELF_TEST_LEVEL(l) __declspec(allocate(l))

#define SELF_TEST(n,l) \
	extern int SELF_TEST_FUNC self_test_##n(self_test_report_pf); \
	static const char SELF_TEST_RO self_test_msg_##n[] = \
		"self-test: info: test " #n; \
	static struct self_test SELF_TEST_RO self_test_desc_##n = \
		{ self_test_##n, self_test_msg_##n }; \
	struct self_test SELF_TEST_LEVEL(l) *self_test_ptr_##n = \
		&self_test_desc_##n; \
	int SELF_TEST_FUNC self_test_##n(self_test_report_pf self_test_report)

#define SELF_TEST_ASSERT(x) \
	do { \
		if (!(x)) { \
			static const char SELF_TEST_RO message[] = \
				"error: ST0001: self-test failed assertion: " #x;\
			static const char SELF_TEST_RO file[] = __FILE__;\
			self_test_report(message, file, __LINE__); goto failure; \
		} \
	} while(0)

#endif /* WIN32_SELFTEST_H */
