#include "../uthash/include/utarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * utarray is a dynamic array implementation with both data structure and supporting functions. 
 * It provides almost all functionality that vector has in C++, including:
 * 1. Create an array: utarray_new
 * 2. push_back: utarray_push_back
 * 3. pop_back: utarray_pop_back
 * 4. insert: utarray_insert & utarray_inserta
 * 5. erase: utarray_erase
 * 6. sort: utarray_sort
 * 7. find: utarray_find (must be sorted)
 * 8. index_of: utarray_eltidx
 * 9. position based access: utarray_eltptr
 * 
 * To apply for different types, you need to provide a customized UT_icd, which contains
 * the initiator, copy function, and destructor for your data type. 
*/

void print_int_array(UT_array *int_array){
    int *p;
    for(p = (int*)utarray_front(int_array); p != NULL; p = (int*)utarray_next(int_array, p)){
        printf("%d, ", *p);
    }
    printf("\n");
}

// compare function for int. For your own data type, make sure 
// the parameters are const void* since internally the sort call 
// qsort from stdlib.h, which requires compar with two const void *
// type parameter:
// void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
int int_cmp(const void *a, const void *b) {
    int _a = *(const int *)a;
    int _b = *(const int *)b;
    return (_a < _b) ? -1 : (_a > _b);
}

int main(){

    /*
     * First we start with a simple example of int array
     * int array is the simplest case, you don't need to specify the type
     * but the UT_icd 
    */

    UT_array *int_array;
    // Initialize the array, ut_int_icd is a predefined UT_icd for int
    utarray_new(int_array, &ut_int_icd);

    int i;
    for(i = 0; i < 10; i++){
        // push_back
        utarray_push_back(int_array, &i);
    }
    printf("The int array after push_back is:\n");
    print_int_array(int_array);

    // pop_back
    for(i = 0; i < 5; i++){
        utarray_pop_back(int_array);
    }
    printf("The int array after pop_back is:\n");
    print_int_array(int_array);

    // insert an element at position 2
    int insert_value = 100;
    utarray_insert(int_array, &insert_value, 2);
    printf("The int array after insert is:\n");
    print_int_array(int_array);

    // insert some more elements:
    int insert_values[] = {200, 300, 400};
    for (i = 0; i < 3; i++){
        utarray_insert(int_array, &insert_values[i], 3);
    }
    printf("The int array after more insert is:\n");
    print_int_array(int_array);

    // erase an element at position 3
    utarray_erase(int_array, 3, 1);
    printf("The int array after erase is:\n");
    print_int_array(int_array);

    // erase three elements from position 3
    printf("The int array after erase 3 elements is:\n");
    utarray_erase(int_array, 3, 3);
    print_int_array(int_array);

    // sort the array
    printf("Now the array is unsorted, let's sort it:\n");
    utarray_sort(int_array, int_cmp);
    printf("After sort, the array is:\n");
    print_int_array(int_array);

    // find an element
    int find_value = 100;
    int *find_result = (int*)utarray_find(int_array, &find_value, int_cmp);
    if(find_result != NULL){
        printf("The value %d is found at position %ld\n", find_value, find_result - (int*)utarray_front(int_array));
    } else {
        printf("The value %d is not found\n", find_value);
    }

    // index_of
    // Index of is different from found, it requires the input
    // to be the pointer to the element, and then returns the idx
    // You can't pass in a value's address, in other word, it's 
    // translating memory address to index
    int idx = utarray_eltidx(int_array, find_result);
    printf("The index of the found value %d is %d\n", *find_result, idx);

    // find an element that is not in the array
    find_value = 1000;
    find_result = (int*)utarray_find(int_array, &find_value, int_cmp);
    if(find_result != NULL){
        printf("The value %d is found at position %ld\n", find_value, find_result - (int*)utarray_front(int_array));
    } else {
        printf("The value %d is not found\n", find_value);
    }

    // position based access
    int *p;
    for(i = 0; i < 5; i++){
        p = (int*)utarray_eltptr(int_array, i);
        printf("The value at position %d is %d\n", i, *p);
    }
    
    
}