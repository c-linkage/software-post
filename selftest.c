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

//
// Platform-independent entry point for running self-tests.
//

static const char SELF_TEST_RO self_test_msg_start[] =
	"self-test: info: starting self test...";

static const char SELF_TEST_RO self_test_msg_end[] =
	"self-test: info: self test complete";

static const char SELF_TEST_RO self_test_msg_failed[] =
	"self-test: error: self test failed";

int self_test_run(self_test_report_pf report, unsigned flags)
{
	if (report == NULL) 
		report = sys_self_test_report;
	
	report(self_test_msg_start, NULL, 0);

	if (sys_self_test_run(report, flags))
	{
		report(self_test_msg_end, NULL, 0);
		return 1;
	}
	else
	{
		report(self_test_msg_failed, NULL, 0);
		return 0;
	}
}
