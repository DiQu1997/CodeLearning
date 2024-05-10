# UTArray

## 1. Introduction
UTArray is the array implementation in UT library. It almost implements all the functionality that you can find in c++'s Vector, including create, push_back, pop_back, insert, remove, sort, find, etc. The biggest difference is that it only supports find for sorted data(it's using binary seach)

## 2. Data Structure

The main UT_array structure is defined as follows:

```c
typedef struct {
    unsigned i, n;
    UT_icd icd;
    char *d;
} UT_array;
```

Here `i` is the next available index, and also can represents the size of the array. `n` is the total number of elements that the array can hold. `d` is the pointer to the data array, stored as char array (equivalent to byte array). `icd` is the interface control data:

```c
typedef void (ctor_f)(void *dst, const void *src);
typedef void (dtor_f)(void *elt);
typedef void (init_f)(void *elt);

typedef struct {
    size_t sz;
    UT_icd_initf *init;
    UT_icd_copyf *copy;
    UT_icd_delf *dtor;
} UT_icd;
```

It stores the size of the element, and the function pointers to the constructor, destructor and initializer. This is for your customized data structure. If you have a complex data structure, you need to define your own UT_icd structure and pass it to the UTArray functions.

## 3. Functions

### 3.1. Create, Reset and Destroy

```c
#define utarray_init(a,_icd) do {
  memset(a,0,sizeof(UT_array));
  (a)->icd = *(_icd);
} while(0)

#define utarray_done(a) do {
  if ((a)->n) {
    if ((a)->icd.dtor) {
      unsigned _ut_i;
      for(_ut_i=0; _ut_i < (a)->i; _ut_i++) {
        (a)->icd.dtor(utarray_eltptr(a,_ut_i));
      }
    }
    free((a)->d);
  }
  (a)->n=0;
} while(0)

#define utarray_new(a,_icd) do {
  (a) = (UT_array*)malloc(sizeof(UT_array));
  if ((a) == NULL) {
    utarray_oom();
  }
  utarray_init(a,_icd);
} while(0)

#define utarray_free(a) do {
  utarray_done(a);
  free(a);
} while(0)
```

TBH the name is a little ambiguous. `utarray_init` is used to initialize an array, and `utarray_new` is used to create a new array. You need to create the new array using `utarray_new` first and then call `utarray_init` to reset the value of the array. While you should call `utarray_free` to free the memory of the array. It will use the destructor function to free the memory of each element.

### 3.2. push_back, pop_back, extent_back(emplace) insert and remove
```c
#define utarray_push_back(a,p) do {                                           
  utarray_reserve(a,1);                                                       
  if ((a)->icd.copy) { (a)->icd.copy( _utarray_eltptr(a,(a)->i++), p); }      
  else { memcpy(_utarray_eltptr(a,(a)->i++), p, (a)->icd.sz); };              
} while(0)

#define utarray_pop_back(a) do {                                              
  if ((a)->icd.dtor) { (a)->icd.dtor( _utarray_eltptr(a,--((a)->i))); }       
  else { (a)->i--; }                                                          
} while(0)

#define utarray_extend_back(a) do {
  utarray_reserve(a,1);
  if ((a)->icd.init) { (a)->icd.init(_utarray_eltptr(a,(a)->i)); }
  else { memset(_utarray_eltptr(a,(a)->i),0,(a)->icd.sz); }
  (a)->i++; 
} while(0)

#define utarray_insert(a,p,j) do {                                            
  if ((j) > (a)->i) utarray_resize(a,j);                                      
  utarray_reserve(a,1);                                                       
  if ((j) < (a)->i) {                                                         
    memmove( _utarray_eltptr(a,(j)+1), _utarray_eltptr(a,j),                  
             ((a)->i - (j))*((a)->icd.sz));                                   
  }                                                                           
  if ((a)->icd.copy) { (a)->icd.copy( _utarray_eltptr(a,j), p); }             
  else { memcpy(_utarray_eltptr(a,j), p, (a)->icd.sz); };                     
  (a)->i++;                                                                   
} while(0)

#define utarray_erase(a,pos,len) do {
  if ((a)->icd.dtor) {
    unsigned _ut_i;
    for (_ut_i = 0; _ut_i < (len); _ut_i++) {
      (a)->icd.dtor(utarray_eltptr(a, (pos) + _ut_i));
    }
  }
  if ((a)->i > ((pos) + (len))) {
    memmove(_utarray_eltptr(a, pos), _utarray_eltptr(a, (pos) + (len)),
            ((a)->i - ((pos) + (len))) * (a)->icd.sz);\
  }
  (a)->i -= (len);
} while(0)
```

`utarray_push_back` is used to push an element to the end of the array (where int i points to). It will call the copy function to copy the element to the array. If your data structure is simple (like int, float, or simple struct without pointers), you can use define the copy constructor as null. The macro will cal memcpy to copy the element to the pointer while `utarray_extent_back` is just used to extend the array and call the initializer function to initialize the element or set it to 0 (push_back an empty value). `utarray_pop_back` is just doing the reverse way. One thing to notify is that push_back and extent_back both will call `utarray_reserve` to check if the array has enough space to store the new element. If not, it will call `realloc` to extend the array. According to [cppreference](https://en.cppreference.com/w/c/memory/realloc)
```
The reallocation is done by either:

a) expanding or contracting the existing area pointed to by ptr, if possible. The contents of the area remain unchanged up to the lesser of the new and old sizes. If the area is expanded, the contents of the new part of the array are undefined.
b) allocating a new memory block of size new_size bytes, copying memory area with size equal the lesser of the new and the old sizes, and freeing the old block.
```

`utarray_insert` is used to insert an element to the array at the position j. Like any array-based implementation, it will move the elements after j to the right(by calling memmove) and then insert the element to the position. `utarray_erase` is used to remove the element from the array. It will call the destructor to free the memory of the element and use memmove to move the elements after the position to the left.

### 3.3. Find and Sort

```c
#define utarray_sort(a,cmp) do {
  qsort((a)->d, (a)->i, (a)->icd.sz, cmp);
} while(0)

#define utarray_find(a,v,cmp) bsearch((v),(a)->d,(a)->i,(a)->icd.sz,cmp)

```

Find is only supported for sorted data. It will use the binary search to find the element in the array. Sort is using the qsort function to sort the array. You need to pass the compare function for the element to sort the array. 

# Summary 

utarray is a simple array implementation in the UT library. It's easy to use and has almost all the functionality that you can find in c++'s vector. The only thing you need to do is to define the UT_icd structure for your data structure and pass it to the UTArray functions. 
