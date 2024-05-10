#include "../uthash/include/utlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 20

// You can use any structure with these macros, as long as the 
// structure contains a next pointer. If you want to make a doubly-linked list, 
// the element also needs to have a prev pointer.

typedef struct el_sl {
    int id;
    char bname[BUFLEN];
    struct el_sl *next;
} el_sl;

static int el_sl_cmp(el_sl *a, el_sl *b) {
    if (a->id < b->id) return -1;
    if (a->id > b->id) return 1;
    return 0;
}

typedef struct el_dl {
    int id;
    char bname[BUFLEN];
    struct el_dl *next;
    struct el_dl *prev;
} el_dl;

typedef struct el_cl {
    int id;
    char bname[BUFLEN];
    struct el_cl *next;
    struct el_cl *prev;
} el_cl;

/*
 * Operations supported:
 * 1. Prepend: Add a new element at the beginning of the list / an specific element 
 * 2. Append: Add a new element at the end of the list / an specific element
 * 3. Insert in order: Add a new element in order (if the list is already sorted)
 * 4. Delete: Remove an element from the list
 * 5. Search: Find an element in the list
 * 6. Count: Count the number of elements in the list
 * 7. Sort: Sort the list
 * 8. For each: Apply a function to each element in the list
 */

void print_LL(el_sl *head) {
    el_sl *el;
    int count = 0;
    LL_COUNT(head, el, count);
    printf("Number of elements: %d\n", count);
    LL_FOREACH(head, el){
        printf("Node:[id = %d, bname = %s]\n -> ", el->id, el->bname);
    }
    printf("\n");
}

int main() {
    /*
     * Single linked list
     * Prepend is O(1), appending, deleting, and insertion in order are O(n)
     * Sorting is O(n log n)
     */

    // Declare the head of the list
    el_sl *head = NULL;

    // To add an element to the list, utlist requires you to create the struct first
    el_sl *new_el = (el_sl *)malloc(sizeof(el_sl));
    new_el->id = 0;
    strncpy(new_el->bname, "new element 0", BUFLEN);
    LL_PREPEND(head, new_el);

    printf("After adding the first element\n");
    print_LL(head);

    // Let's add a couple more elements
    for (int i = 1; i < 5; i++) {
        el_sl *new_el = (el_sl *)malloc(sizeof(el_sl));
        snprintf(new_el->bname, BUFLEN, "new element %d", i);
        new_el->id = i;
        LL_APPEND(head, new_el);
    }
    printf("After adding more elements\n");
    print_LL(head);

    // Let's search and delete the element with id = 2
    el_sl *el;
    LL_SEARCH_SCALAR(head, el, id, 2);
    if (el != NULL) {
        LL_DELETE(head, el);
        free(el);
    }
    printf("After deleting element with id = 2\n");
    print_LL(head);


    // Let's append an element at the end of the list
    new_el = (el_sl *)malloc(sizeof(el_sl));
    new_el->id = 6;
    strncpy(new_el->bname, "new element 6", BUFLEN);
    LL_PREPEND(head, new_el);
    printf("After adding element with id = 6\n");
    print_LL(head);

    // Let's sort it
    LL_SORT(head, el_sl_cmp);
    printf("After sorting\n");
    print_LL(head);
}   