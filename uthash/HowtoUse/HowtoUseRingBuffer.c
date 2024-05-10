#include "../uthash/include/utringbuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[20];
} Person;

UT_icd Person_icd = {sizeof(Person), NULL, NULL, NULL};

int main() {
    UT_ringbuffer *rb;
    Person p1 = {1, "John"};
    Person p2 = {2, "Alice"};
    Person p3 = {3, "Bob"};
    Person p4 = {4, "Emma"};
    Person *ptr;

    // Initialize the ring buffer with a capacity of 3 elements
    utringbuffer_new(rb, 3, &Person_icd);

    // Check if the ring buffer is empty
    printf("Is ring buffer empty? %s\n", utringbuffer_empty(rb) ? "Yes" : "No");

    // Push elements into the ring buffer
    utringbuffer_push_back(rb, &p1);
    utringbuffer_push_back(rb, &p2);
    utringbuffer_push_back(rb, &p3);

    // Check if the ring buffer is full
    printf("Is ring buffer full? %s\n", utringbuffer_full(rb) ? "Yes" : "No");

    // Get the number of elements in the ring buffer
    printf("Number of elements: %d\n", utringbuffer_len(rb));

    // Iterate over the elements in the ring buffer
    printf("Elements in the ring buffer:\n");
    for (ptr = utringbuffer_front(rb); ptr != NULL; ptr = utringbuffer_next(rb, ptr)) {
        printf("ID: %d, Name: %s\n", ptr->id, ptr->name);
    }

    // Push another element into the full ring buffer (overwrite the oldest element)
    utringbuffer_push_back(rb, &p4);

    // Iterate over the elements in the ring buffer
    printf("Elements in the ring buffer:\n");
    for (ptr = utringbuffer_front(rb); ptr != NULL; ptr = utringbuffer_next(rb, ptr)) {
        printf("ID: %d, Name: %s\n", ptr->id, ptr->name);
    }

    // Get the element at a specific index
    ptr = utringbuffer_eltptr(rb, 1);
    if (ptr != NULL) {
        printf("Element at index 1 - ID: %d, Name: %s\n", ptr->id, ptr->name);
    }

    // Get the index of a specific element
    Person *p = &p3;
    int idx = utringbuffer_eltidx(rb, p);
    printf("Index of element p3: %d\n", idx);

    // Iterate over the elements in reverse order
    printf("Elements in reverse order:\n");
    for (ptr = utringbuffer_back(rb); ptr != NULL; ptr = utringbuffer_prev(rb, ptr)) {
        printf("ID: %d, Name: %s\n", ptr->id, ptr->name);
    }

    // Clear the ring buffer
    utringbuffer_clear(rb);

    // Check if the ring buffer is empty after clearing
    printf("Is ring buffer empty after clearing? %s\n", utringbuffer_empty(rb) ? "Yes" : "No");

    // Free the ring buffer
    utringbuffer_free(rb);

    return 0;
}
