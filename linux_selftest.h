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
#ifndef LINUX_SELFTEST_H
#define LINUX_SELFTEST_H

// Use the default calling convention

#define SELF_TEST_DECL

// Setup the names of the required sections for each test level

#define SELF_TEST_LEVEL_1 ".slftst.ini0"
#define SELF_TEST_LEVEL_2 ".slftst.ini1"
#define SELF_TEST_LEVEL_3 ".slftst.ini2"
#define SELF_TEST_LEVEL_4 ".slftst.ini3"
#define SELF_TEST_LEVEL_5 ".slftst.ini4"
#define SELF_TEST_LEVEL_6 ".slftst.ini5"
#define SELF_TEST_LEVEL_7 ".slftst.ini6"
#define SELF_TEST_LEVEL_8 ".slftst.ini7"
#define SELF_TEST_LEVEL_9 ".slftst.ini8"
#define SELF_TEST_LEVEL_10 ".slftst.ini9"

// Set the default test level

#define SELF_TEST_LEVEL_DEFAULT SELF_TEST_LEVEL_5

// Forward declare all sections; unused sections are removed by the linker

extern void __attribute__((__section__(".slftst.inis"))) *self_test_list_start;
extern void __attribute__((__section__(SELF_TEST_LEVEL_1))) *self_test_list_1;
extern void __attribute__((__section__(SELF_TEST_LEVEL_2))) *self_test_list_2;
extern void __attribute__((__section__(SELF_TEST_LEVEL_3))) *self_test_list_3;
extern void __attribute__((__section__(SELF_TEST_LEVEL_4))) *self_test_list_4;
extern void __attribute__((__section__(SELF_TEST_LEVEL_5))) *self_test_list_5;
extern void __attribute__((__section__(SELF_TEST_LEVEL_6))) *self_test_list_6;
extern void __attribute__((__section__(SELF_TEST_LEVEL_7))) *self_test_list_7;
extern void __attribute__((__section__(SELF_TEST_LEVEL_8))) *self_test_list_8;
extern void __attribute__((__section__(SELF_TEST_LEVEL_9))) *self_test_list_9;
extern void __attribute__((__section__(SELF_TEST_LEVEL_10))) *self_test_list_10;
extern void __attribute__((__section__(".slftst.inie"))) *self_test_list_end;

// Define platform-specific macros 

#define SELF_TEST_FUNC \
	__attribute__((__section__(".slftst.txt"))) SELF_TEST_DECL
#define SELF_TEST_RO \
	__attribute__((__used__,__section__(".slftst.str")))
#define SELF_TEST_LEVEL(l) \
	__attribute__((__used__,__section__(l))) 

#define SELF_TEST(n,l) \
	extern int SELF_TEST_FUNC self_test_##n(self_test_report_pf);\
	static const char SELF_TEST_RO self_test_msg_##n[] = \
		"self-test: info: test " # n; \
	static const struct self_test SELF_TEST_RO self_test_desc_##n = \
		{ self_test_##n, self_test_msg_##n };\
	static const struct self_test SELF_TEST_LEVEL(l) *self_test_ptr_##n = \
		&self_test_desc_##n;\
	int SELF_TEST_FUNC self_test_##n(self_test_report_pf self_test_report)

#define SELF_TEST_ASSERT(x) \
	do { \
		if (!(x)) { \
			static const char SELF_TEST_RO message[] = \
				"error: self-test failed assertion: " #x; \
			static const char SELF_TEST_RO file[] = __FILE__; \
			self_test_report(message, file, __LINE__); goto failure; \
		} \
	} while(0)

#endif /* LINUX_SELFTEST_H */
