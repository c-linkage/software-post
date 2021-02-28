# Software Power-On Self Test (POST)

This project implements self-test framework for the C Programming Language allowing the developer to create testing routines in the same translation unit as the code, assign testing routines to dependency layers, and ensures the testing code does not affect the size of the program at runtime.

The self-test framework works on x86 and x86_64 architectures under both the Windows and Linux platforms.

## Contents

This project contains the self-test framework and a toy program to demonstrate examples

The self-test framework is implemented in these files:

* selftest.h
* selftest.c
* win32_selftest.h
* win32_selftest.c
* linux_selftest.h
* linux_selftest.c

The toy program is implemented in these files:

* main.c
* mem.h
* mem.c
* list.h
* list.c

**Please review the entire toy program as it demonstrates the full capabilities of this framework.**

## Usage

Include the self-test files in the build process for your application.  

**IMPORTANT: Under Linux, the linux_selftest.o object file must be the first file passed to the linker or the self-test framework will not work.**

Include a call to `self_test_run()` in your main program, preferably before any real work is done by the application.  If the `self_test_run()` function returns zero, at least one self-test failed and you should exit the program immediately to avoid running the program in a potentially corrupted environment.

## Implementing a Self-Test

Self-tests are written directly into the translation unit of the module or subsystem being tested.  This helps to keep the code and the tests synchronized over time.

The following platform-dependent macros have been defined:

* `SELF_TEST(n,l)` - Implement a self-test for module `n` at testing level `l`
* `SELF_TEST_ASSERT(x)` - Assert that expression `x` is true, otherwise report an error and jump to the label `failure`
* `SELF_TEST_FUNC` - Function decoration needed to install testsupport functions into the self-test section
* `SELF_TEST_RO` - Variable decoration needed to install test read-only variables into the read-only self-test section 

Here is an example for setting up and running a basic self-test.
    
    basic_test.c
    
    01: #include "selftest.h"
    02: 
    03: SELF_TEST(basic_test, SELF_TEST_LEVEL_DEFAULT)
    04: {
    05:   int rc = 0;
    06:   int success = 1;
    07:   int failure = 3;
    08:
    09:   SELF_TEST_ASSERT(success == 1);
    10:   SELF_TEST_ASSERT(failure == 0);
    11:   rc = 1;
    12:
    13: failure:
    14:   return rc;
    15: }
    
    main.c

    01: #include <stdio.h>
    02: 
    03: int main()
    04: {
    05:   return self_test_run(NULL, SELF_TEST_FLAG_NONE);
    06: }

The output from running this under Linux will be as follows:

    self-test: info: starting self test...
    self-test: info: test basic_test
    basic_test.c(10): error: self-test failed assertion: failure == 0
    self-test: error: self test failed

The output from running this under Windows will be as follows:

    self-test: info: starting self test...
    self-test: info: test basic_test
    basic_test.c:10: error: ST0001: self-test failed assertion: failure == 0
    self-test: error: self test failed
    
Note that the messages are formatted to be consistent with the message styles used on their respecive platforms. IDEs used on these platforms should be able to parse these messages and navigate directly to the self-test asserion that failed.
