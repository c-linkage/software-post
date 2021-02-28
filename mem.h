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
#ifndef MEM_H
#define MEM_H

#include <stddef.h>

// Define the function used to report memory leaks

typedef void (*mem_report_pf)(const char *file, int line, void *data);

// Initialize the memory subsystem

extern void mem_init(void);

// Uninitialize the memory subsystem and report leaks.
// The report function is used to report the leaks.  
// The data value is passed to the report function unmodified to inject 
// a dependency such as a file handle.

extern int mem_uninit(mem_report_pf report, void *data);

// Macros to allocate and release memory

#define mem_alloc(s) mem_alloc_internal((s), __FILE__, __LINE__)
#define mem_free(p) mem_free_internal(p)
#define mem_create(s) (s *)mem_alloc(sizeof(s))

// Do not call these internal functions directly

extern void *mem_alloc_internal(size_t size, const char *file, int line);
extern void mem_free_internal(void *ptr);

#endif /* MEM_H */
