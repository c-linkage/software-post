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
#ifndef SELFTEST_H
#define SELFTEST_H

#include <stddef.h>

#if defined(_WIN32)
#include "win32_selftest.h"
#else
#include "linux_selftest.h"
#endif

//
// Definition of function that reports self-test errors.
//
typedef void (SELF_TEST_DECL *self_test_report_pf)(
	const char *msg, 		// Message to report
	const char *file, 		// Filename from which error is reported
	size_t line				// Line number in file reporting error
);

//
// Definition of function that drives a unit- or subsystem-specific test
// sequence and reports errors using the specified reporting function.
//
typedef int (SELF_TEST_DECL *self_test_pf)(
	self_test_report_pf report	// Pointer to function that reports errors
);

//
// Forward declarations for system-dependent support functions.
//

extern void sys_self_test_report(
	const char *message, const char *file, size_t line
);

extern int sys_self_test_run(
	self_test_report_pf report, unsigned flags
);

//
// Self-test structure binding a name to a driver function.
//

struct self_test
{
	self_test_pf	func;	// Driver function
	const char		*name;	// Name of the self test
};

enum {
	SELF_TEST_FLAG_NONE = 0,				// No flag
	SELF_TEST_FLAG_STOP_ON_FAILURE = 1		// Stop self test on error
};

#define SELF_TEST_SYSTEM_REPORT NULL

//
// Main driver function that runs all defined self tests.
//
// All self-tests are executed using the same reporting functions and
// self-test flags.
//
// The report parameter is optional. When NULL, this function uses
// the default platform-specific reporting function sys_self_test_report.
//
extern int self_test_run(self_test_report_pf report, unsigned flags);

//
// CREATING A SELF-TEST
//
// The following platform-dependent macros have been defined for use in
// creating a self-test:
//
// SELF_TEST(n,l) - Define a self test function 'n' to operate at level 'l'
// SELF_TEST_ASSERT(x) - Jump to local label 'failure' when 'x' is not true
// SELT_TEST_RO - Variable decoration needed to install the variable into
//                the read-only self-test section
//
// EXAMPLE
//
// basic_test.c
//
// 01: #include "selftest.h"
// 02: 
// 03: SELF_TEST(basic_test, SELF_TEST_LEVEL_DEFAULT)
// 04: {
// 05:   int rc = 0;
// 06:   int success = 1;
// 07:   int failure = 3;
// 08:
// 09:   SELF_TEST_ASSERT(success == 1);
// 10:   SELF_TEST_ASSERT(failure == 0);
// 11:   rc = 1;
// 12:
// 13: failure:
// 14:   return rc;
// 15: }
//
// main.c
//
// 01: #include <stdio.h>
// 02: 
// 03: int main()
// 04: {
// 05:   return self_test_run(NULL, SELF_TEST_FLAG_NONE);
// 06: }
//
// LINUX OUTPUT 
//
// self-test: info: starting self test...
// self-test: info: test basic_test
// basic_test.c(10): error: self-test failed assertion: failure == 0
// self-test: error: self test failed
//
// WINDOWS OUTPUT
// 
// self-test: info: starting self test...
// self-test: info: test basic_test
// basic_test.c:10: error: ST0001: self-test failed assertion: failure == 0
// self-test: error: self test failed
//
//

#endif /* SELFTEST_H */
