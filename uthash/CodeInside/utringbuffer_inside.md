# UT Ringbuffer

## 1. What is Ring Buffer:
A ring buffer, also known as a circular buffer, is a data structure that uses a fixed-size array to efficiently store and manage elements in a circular manner. It is particularly useful in scenarios where you need to process a continuous stream of data or when you have a fixed amount of memory available.

In a ring buffer, the array is treated as a circular structure, where the end of the array wraps around to the beginning. This allows for efficient insertion and removal of elements without the need for shifting elements or resizing the array. When the buffer is full and a new element is inserted, it overwrites the oldest element in the buffer, making room for the new one.

Ring buffers are commonly used in various applications, such as:

<ul>
    <li> Buffering data between producers and consumers in multi-threaded or distributed systems.
    <li> Implementing fixed-size caches or history buffers.
    <li> Handling streaming data or real-time data processing.
    <li> Implementing circular queues or FIFOs (First-In-First-Out) data structures.
</ul>

Compared to other data structures with similar functionality, such as dynamic arrays or linked lists, ring buffers offer several advantages:

<ul>
    <li> Fixed memory usage: Ring buffers operate on a fixed-size array, which means they have a predictable and constant memory footprint. This is particularly beneficial in resource-constrained environments or when memory allocation needs to be minimized.
    <li> Efficient insertion and removal: Inserting and removing elements from a ring buffer has a time complexity of O(1), as it only involves updating the buffer's pointers or indices. This makes ring buffers highly efficient for handling continuous data streams or frequent element updates.
</ul>

It is important to note that ring buffers also have some limitations, such as:
<ul>
    <li> Fixed capacity: The capacity of a ring buffer is determined during initialization and cannot be changed dynamically. If the buffer becomes full and new elements need to be inserted, the oldest elements will be overwritten. This can be a limitation in scenarios where the required buffer size is not known in advance or may vary over time.
</ul>


## 2. UT Ringbuffer:

UTRingBuffer is a ring buffer implementation in the UT library using macros in C. A ring buffer, also known as a circular buffer, is a data structure that efficiently stores and manages elements in a circular manner using a fixed-size array. It is particularly useful in scenarios where you need to process a continuous stream of data or when you have a fixed amount of memory available.

The UTRingBuffer implementation is designed to be lightweight, efficient, and easy to use. It utilizes macros to provide a concise and readable interface for interacting with the ring buffer. By leveraging macros, the implementation avoids the overhead of function calls and allows for flexibility in terms of element types and buffer sizes.

## 3. Data Structure
The main UT_ringbuffer structure is defined as follows:

```c

typedef struct {
    unsigned i;       /* index of next available slot; wraps at n */
    unsigned n;       /* capacity */
    unsigned char f;  /* full */
    UT_icd icd;       /* initializer, copy and destructor functions */
    char *d;          /* n slots of size icd->sz */
} UT_ringbuffer;
```

Here i is the index of the next available slot in the buffer, and it wraps around to the beginning of the buffer when it reaches the capacity. n represents the capacity of the ring buffer, i.e., the maximum number of elements it can hold. f is an unsigned char flag that indicates whether the buffer is full or not. It is set to 1 when the buffer is full and 0 otherwise. d is a character pointer representing the underlying array that holds the actual elements of the ring buffer. It is dynamically allocated based on the capacity and the size of each element.

icd is the interface control data, defined as:

```c
typedef struct {
    size_t sz;
    init_f *init;
    ctor_f *copy;
    dtor_f *dtor;
} UT_icd;
```
It stores the size of the element (sz) and function pointers for initializing (init), copying (copy), and destroying (dtor) elements. These function pointers can be customized based on the specific element type being stored in the ring buffer. If you have a complex data structure, you need to define your own UT_icd structure and pass it to the UTRingBuffer macros.

The UTRingBuffer's data structure is designed to be simple yet efficient. The combination of the i, n, and f fields allows for easy tracking of the buffer's state and facilitates the circular wrapping behavior. The icd field provides flexibility in handling different element types, while the d field holds the actual data stored in the buffer.

## 4. Functions
The UTRingBuffer provides a set of macros for initializing, pushing, popping, accessing, and iterating over elements in the ring buffer. These macros are designed to be concise and efficient, leveraging the power of C preprocessor macros to provide a flexible and easy-to-use interface for working with ring buffers.

### 4.1. Initialization and Destruction
```c
#define utringbuffer_init(a, _n, _icd) do {
  memset(a, 0, sizeof(UT_ringbuffer));
  (a)->icd = *(_icd);
  (a)->n = (_n);
  if ((a)->n) { (a)->d = (char*)malloc((a)->n * (_icd)->sz); }
} while(0)

#define utringbuffer_done(a) do {
  utringbuffer_clear(a);
  free((a)->d); (a)->d = NULL;
  (a)->n = 0;
} while(0)
```

utringbuffer_init is a macro used to initialize a UTRingBuffer. It takes three arguments: a (a pointer to the UTRingBuffer), _n (the capacity of the buffer), and _icd (a pointer to the UT_icd structure). The macro sets the icd field of the buffer to the provided _icd, sets the capacity n to _n, and allocates memory for the underlying array d based on the capacity and element size.

utringbuffer_done is a macro used to clean up and destroy a UTRingBuffer. It calls utringbuffer_clear to clear the contents of the buffer, frees the memory allocated for the underlying array d, sets d to NULL, and sets the capacity n to 0.

### 4.2. Pushing and Popping Elements
```c
#define utringbuffer_push_back(a,p) do {
  if ((a)->icd.dtor && (a)->f) { (a)->icd.dtor(_utringbuffer_internalptr(a,(a)->i)); }
  if ((a)->icd.copy) { (a)->icd.copy( _utringbuffer_internalptr(a,(a)->i), p); }
  else { memcpy(_utringbuffer_internalptr(a,(a)->i), p, (a)->icd.sz); };
  if (++(a)->i == (a)->n) { (a)->i = 0; (a)->f = 1; }
} while(0)
```
utringbuffer_push_back is a macro used to push an element to the back of the UTRingBuffer. It takes two arguments: a (a pointer to the UTRingBuffer) and p (a pointer to the element to be pushed). The macro first checks if the buffer is full and the destructor function is provided. If so, it calls the destructor on the element at the current index i. Then, it either calls the copy function (if provided) or uses memcpy to copy the element pointed by p to the current index i. Finally, it increments the index i and wraps it around to 0 if it reaches the capacity, and sets the f flag to 1 if the buffer becomes full.

### 4.3. Accessing Elements
```c
#define utringbuffer_front(a) utringbuffer_eltptr(a,0)
#define utringbuffer_back(a) (utringbuffer_empty(a) ? NULL : utringbuffer_eltptr(a, utringbuffer_len(a) - 1))

#define utringbuffer_eltptr(a,j) ((0 <= (j) && (j) < utringbuffer_len(a)) ? _utringbuffer_internalptr(a,_utringbuffer_real_idx(a,j)) : NULL)

#define utringbuffer_eltidx(a,e) _utringbuffer_fake_idx(a, _utringbuffer_internalidx(a,e))
```
utringbuffer_front is a macro that returns a pointer to the front element of the UTRingBuffer. It calls utringbuffer_eltptr with an index of 0.

utringbuffer_back is a macro that returns a pointer to the back element of the UTRingBuffer. It first checks if the buffer is empty using utringbuffer_empty. If so, it returns NULL. Otherwise, it calls utringbuffer_eltptr with an index of utringbuffer_len(a) - 1.

utringbuffer_eltptr is a macro that returns a pointer to the element at a given index j in the UTRingBuffer. It first checks if the index is within the valid range (between 0 and the length of the buffer). If so, it calls _utringbuffer_internalptr with the real index obtained from _utringbuffer_real_idx. Otherwise, it returns NULL.

utringbuffer_eltidx is a macro that returns the fake (logical) index of a given element pointer e in the UTRingBuffer. It calls _utringbuffer_fake_idx with the internal index obtained from _utringbuffer_internalidx.

### 4.4. Iterating Elements
```c
#define utringbuffer_next(a,e) ((e)==NULL ? utringbuffer_front(a) : utringbuffer_eltptr(a, utringbuffer_eltidx(a,e)+1))
#define utringbuffer_prev(a,e) ((e)==NULL ? utringbuffer_back(a) : utringbuffer_eltptr(a, utringbuffer_eltidx(a,e)-1))
```
utringbuffer_next is a macro that returns a pointer to the next element after a given element pointer e in the UTRingBuffer. If e is NULL, it returns the front element using utringbuffer_front. Otherwise, it calls utringbuffer_eltptr with the index obtained by incrementing the fake index of e using utringbuffer_eltidx.

utringbuffer_prev is a macro that returns a pointer to the previous element before a given element pointer e in the UTRingBuffer. If e is NULL, it returns the back element using utringbuffer_back. Otherwise, it calls utringbuffer_eltptr with the index obtained by decrementing the fake index of e using utringbuffer_eltidx.

These macros provide a way to iterate over the elements in the UTRingBuffer in both forward and backward directions.

The UTRingBuffer API uses macros extensively to provide a concise and efficient interface for working with ring buffers. By leveraging macros, the implementation avoids the overhead of function calls and allows for flexibility in terms of element types and buffer sizes. The macros handle the circular nature of the buffer, manage memory allocation and deallocation, and provide convenient access to elements through pointers and indices.

### 4.5 Internal Transform between Logical and Real Index
```c
#define _utringbuffer_real_idx(a,j) ((a)->f ? ((j) + (a)->i) % (a)->n : (j))
#define _utringbuffer_fake_idx(a,j) ((a)->f ? ((j) + (a)->n - (a)->i) % (a)->n : (j))
```

The math behind the function is that:
when we transform from logical idx to real idx: 
```
real_idx = (buffer_full ? ((logical_idx + next_available_idx) % capacity) : logical_idx)
```
when we transform from real idx to logical idx:
```
logical_idx = (buffer_full ? ((real_idx + capacity - next_available_idx) % capacity) : real_idx)
```

Starting from logical idx to real idx:
1. When the buffer is full, the next available index is the head of the buffer, The logical index represents the position of an element relative to the front of the buffer, as if the elements were in a contiguous sequence. To find the corresponding real index, we need to account for the circular nature of the buffer and the position of the next_available_idx.
2. By adding the logical index to the next_available_idx, we effectively shift the logical index based on the current position of the front of the buffer.
3. However, this sum might exceed the capacity of the buffer. To handle the wrapping around, we take the modulo of the sum with the capacity.
4. The modulo operation % capacity ensures that the resulting real index always falls within the valid range of indices in the underlying array.

Vice versa, to get logical idx from real idx: 
1. The real index represents the actual position of an element in the underlying array.
2. To find the corresponding logical index, we need to account for the circular nature of the buffer and the position of the next_available_idx.By adding the capacity to the real index and subtracting the next_available_idx, we effectively shift the real index based on the current position of the front of the buffer.
3. The addition of capacity ensures that the result is always non-negative, handling the case when the real index is smaller than the next_available_idx.
4. However, this sum might exceed the capacity of the buffer. To handle the wrapping around, we take the modulo of the result with the capacity.
5. The modulo operation % capacity ensures that the resulting logical index always falls within the valid range of indices in the logical sequence.

## 5 Conclusion:
The UTRingBuffer implementation offers a simple and efficient solution for managing circular buffers in C. By using macros, it provides a concise and readable interface while avoiding the overhead of function calls. The use of real and logical indices allows for intuitive access to elements and efficient memory management. Overall, UTRingBuffer is a lightweight and flexible option for handling ring buffers in C applications.