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

#ifndef LIST_H
#define LIST_H

// Define the opaque data type `list` that uses a hidden type called `link`
// that implements the linked list.

struct list { struct link *next; };

// Initialize a list 
extern void list_init(struct list *list);

// Return the number of entries in the list
extern size_t list_count(struct list *list);

// Add an integer to the list
extern int list_add(struct list *list, int value);

// Remove an integer from the list; do nothing if the integer is not found
extern void list_remove(struct list *link, int value);

// Return 1 if the list contains the specified integer; 0 otherwise
extern int list_contains(struct list *list, int value);

// Empty the contents of the list and return it to the initialized state
extern void list_clear(struct list *list);

#endif /* LIST_H */
