# UTList

## 1. Introduction
UTList is an header-file only library that provides a set of macros to manipulate linked lists. It does not create the linked list structure itself, but a set of helper function (macro) that help to manipulate the linked list, the struct with data and pointer `next` to the next element( and also pointer `prev` that point to last element, for double linked list) that you have to define yourself.

All "functions" are macros implemented in header files. Therefore it will be replaced by the preprocessor before the compilation of the code. This means that the code will be more efficient than using a function, but it will also be less readable.
 * Operations supported:
 * 1. Prepend: Add a new element at the beginning of the list / an specific element 
 * 2. Append: Add a new element at the end of the list / an specific element
 * 3. Insert in order: Add a new element in order (if the list is already sorted)
 * 4. Delete: Remove an element from the list
 * 5. Search: Find an element in the list
 * 6. Count: Count the number of elements in the list
 * 7. Sort: Sort the list
 * 8. For each: Apply a function to each element in the list

## 2. How to use
To use UTList, you have to define a struct that contains the data and the pointer to the next element. For example, if you want to create a list of integers, you can define the struct as follows:
```c
typedef struct int_list {
    int data;
    struct int_list *next;
} int_list;
```
Then, you can use the UTList macros to manipulate the list. For example, to append an element to the list, you can use the `UTLIST_APPEND` macro as follows:
```c
int_list *list = NULL;
int_list *new_element = malloc(sizeof(int_list));
new_element->data = 42;
UTLIST_APPEND(list, new_element);
```
From this example you can also understand that utlist is only providing the helper functions, you have to manage the memory yourself.

For more example you can refer to HowtoUseList.c file in HowtoUse folder.

## 3. Code Walkthrough

### 3.1. Append. Prepend

```c
#define LL_APPEND(head,add)
    LL_APPEND2(head,add,next)

#define LL_APPEND2(head,add,next)
do {
  LDECLTYPE(head) _tmp;
  (add)->next=NULL;
  if (head) {
    _tmp = (head);
    while (_tmp->next) { _tmp = _tmp->next; }
    _tmp->next=(add); 
  } else {
    (head)=(add);
  }
} while (0)
```

LL_APPEND2 is a macro that allows user to define the name of "next", for example you have a struct node, instead of call `node->next` you may want to call it `node->next_node` then you can use LL_APPEND2(now, new, next_node) to append the new node to the list. LL_APPEND is a wrapper of LL_APPEND2 that use `next` as the name of the pointer to the next element.

LDECLTYPE is a macro that essentially call __typeof (or other function that is equivalent to __typeof in other OS) to get the type of the variable. It is used to get the type of the head variable and define tmp variable with the same type to iterate the list. After that it's just a generl linked list append operation that find the last element of the list and append the new element to it, or set head to it if the list is empty.

Prepend is similar to append, but it add the new element to the beginning of the list so it's much simpler. All you need is:
```c
  (add)->next = head;
  (head) = (add);
```

### 3.2. foreach, count, search 

```c
#define LL_COUNT(head,el,counter)
    LL_COUNT2(head,el,counter,next)

#define LL_COUNT2(head,el,counter,next)
do {
  (counter) = 0;
  LL_FOREACH2(head,el,next) { ++(counter); }
} while (0)

#define LL_FOREACH(head,el)
    LL_FOREACH2(head,el,next)

#define LL_FOREACH2(head,el,next)
    for ((el) = (head); el; (el) = (el)->next)

#define LL_FOREACH_SAFE(head,el,tmp)
    LL_FOREACH_SAFE2(head,el,tmp,next)

#define LL_FOREACH_SAFE2(head,el,tmp,next)
  for ((el) = (head); (el) && ((tmp) = (el)->next, 1); (el) = (tmp))

#define LL_SEARCH_SCALAR(head,out,field,val)
    LL_SEARCH_SCALAR2(head,out,field,val,next)

#define LL_SEARCH_SCALAR2(head,out,field,val,next)
do {
    LL_FOREACH2(head,out,next) {
      if ((out)->field == (val)) break;
    }
} while (0)

#define LL_SEARCH(head,out,elt,cmp)
    LL_SEARCH2(head,out,elt,cmp,next)

#define LL_SEARCH2(head,out,elt,cmp,next)
do {
    LL_FOREACH2(head,out,next) {
      if ((cmp(out,elt))==0) break;
    }
} while (0)
```

The fundamental element of these macros is `LL_FOREACH2`, which simply just iterate through the list using a for loop (don't forget the {} when you use it!). While this implementation is not "safe" if you want to delete the current element. It will cause `(el) = (el)->next` fail since el now points to null, unless you handled it yourself. Therefore, there is a `LL_FOREACH_SAFE2` that use a tmp variable to store the next element before you delete the current element, and assign el to tmp for next iteration. 

With FOREACH, count, and search is just apply for each like a linear search / count. However you don't want to use it when you need kind of "prev" for LL, like DELETE and LOWER_BOUND. LOWER_BOUND needs you to check if the current element is the larger than the target and return the prev, or if the next element is larger and return the current, while you can do it with FOREACH, it's not efficient.

### 3.3. Delete and Lower Bound

```c
#define LL_INSERT_INORDER(head,add,cmp)
    LL_INSERT_INORDER2(head,add,cmp,next)

#define LL_INSERT_INORDER2(head,add,cmp,next)
do {
  LDECLTYPE(head) _tmp;
  if (head) {
    LL_LOWER_BOUND2(head, _tmp, add, cmp, next);
    LL_APPEND_ELEM2(head, _tmp, add, next);
  } else {
    (head) = (add);
    (head)->next = NULL;
  }
} while (0)

#define LL_LOWER_BOUND(head,elt,like,cmp)
    LL_LOWER_BOUND2(head,elt,like,cmp,next)

#define LL_LOWER_BOUND2(head,elt,like,cmp,next)
  do {
    if ((head) == NULL || (cmp(head, like)) >= 0) {
      (elt) = NULL;
    } else {
      for ((elt) = (head); (elt)->next != NULL; (elt) = (elt)->next) {
        if (cmp((elt)->next, like) >= 0) {
          break;
        }
      }
    }
  } while (0)

#define LL_DELETE(head,del)
    LL_DELETE2(head,del,next)

#define LL_DELETE2(head,del,next)
do {
  LDECLTYPE(head) _tmp;
  if ((head) == (del)) {
    (head)=(head)->next;
  } else {
    _tmp = (head);
    while (_tmp->next && (_tmp->next != (del))) {
      _tmp = _tmp->next;
    }
    if (_tmp->next) {
      _tmp->next = (del)->next;
    }
  }
} while (0)
```

LL_INSERT_INORDER is a macro that insert the new element in order. It first find the lower bound of the new element, then append the new element to the list. LL_LOWER_BOUND is a macro that find the lower bound of the new element, it will return the element that is the largest element that is smaller than the new element. LL_DELETE is a macro that delete the element from the list. It first check if the element is the head, if it is then set the head to the next element, otherwise find the element and set the next of the previous element to the next of the element to be deleted.

### 3.4. Sort

```c
#define LL_SORT(list, cmp)
    LL_SORT2(list, cmp, next)

#define LL_SORT2(list, cmp, next)
do {
  LDECLTYPE(list) _ls_p;
  LDECLTYPE(list) _ls_q;
  LDECLTYPE(list) _ls_e;
  LDECLTYPE(list) _ls_tail;
  IF_NO_DECLTYPE(LDECLTYPE(list) _tmp;)
  int _ls_insize, _ls_nmerges, _ls_psize, _ls_qsize, _ls_i, _ls_looping;
  if (list) {
    _ls_insize = 1;
    _ls_looping = 1;
    while (_ls_looping) {
      UTLIST_CASTASGN(_ls_p,list);
      (list) = NULL;
      _ls_tail = NULL;
      _ls_nmerges = 0;
      while (_ls_p) {
        _ls_nmerges++;
        _ls_q = _ls_p;
        _ls_psize = 0;
        for (_ls_i = 0; _ls_i < _ls_insize; _ls_i++) {
          _ls_psize++;
          UTLIST_SV(_ls_q,list); _ls_q = UTLIST_NEXT(_ls_q,list,next); UTLIST_RS(list);
          if (!_ls_q) break;
        }
        _ls_qsize = _ls_insize;
        while (_ls_psize > 0 || (_ls_qsize > 0 && _ls_q)) {
          if (_ls_psize == 0) {
            _ls_e = _ls_q; UTLIST_SV(_ls_q,list); _ls_q =
              UTLIST_NEXT(_ls_q,list,next); UTLIST_RS(list); _ls_qsize--;
          } else if (_ls_qsize == 0 || !_ls_q) {
            _ls_e = _ls_p; UTLIST_SV(_ls_p,list); _ls_p =
              UTLIST_NEXT(_ls_p,list,next); UTLIST_RS(list); _ls_psize--;
          } else if (cmp(_ls_p,_ls_q) <= 0) {
            _ls_e = _ls_p; UTLIST_SV(_ls_p,list); _ls_p =
              UTLIST_NEXT(_ls_p,list,next); UTLIST_RS(list); _ls_psize--;
          } else {
            _ls_e = _ls_q; UTLIST_SV(_ls_q,list); _ls_q =
              UTLIST_NEXT(_ls_q,list,next); UTLIST_RS(list); _ls_qsize--;
          }
          if (_ls_tail) {
            UTLIST_SV(_ls_tail,list); UTLIST_NEXTASGN(_ls_tail,list,_ls_e,next); UTLIST_RS(list);
          } else {
            UTLIST_CASTASGN(list,_ls_e);
          }
          _ls_tail = _ls_e;
        }
        _ls_p = _ls_q;
      }
      if (_ls_tail) {
        UTLIST_SV(_ls_tail,list); UTLIST_NEXTASGN(_ls_tail,list,NULL,next); UTLIST_RS(list);
      }
      if (_ls_nmerges <= 1) {
        _ls_looping=0;
      }
      _ls_insize *= 2;
    }
  }
} while (0)
```

LL_SORT is a macro that sort the list using merge sort. Instead of the top-down recursive implementation we learned in the class, this implementation is bottom-up. This is what they called Simon Tatham's merge sort. Although the code looks complex, the idea is simple. It first merge 1 element, then 2 elements, then 4 elements, and so on until the list is sorted. It's a bit hard to understand because it's not using the recursive call, but it's just a loop that merge the list in order. In pseudo code it's like this:
```
sublist_size = 1
while sublist_size < n
  for i = 0 to n - 1 step 2 * sublist_size
    left = i
    left_size = min(sublist_size, n - i)
    right = min(i + sublist_size - 1, n - 1)
    right_size = min(sublist_size, n - i - sublist_size)
    merge(left, left_size, right, right_size)
  sublist_size *= 2
```

Here the _ls_p is the left, _ls_q is the right. First loop `while (_ls_looping)` is loop over the list until the list is sorted. You can see _ls_insize, defined out of the loop, multiply by 2 each time. In side that you have `while(_ls_p)` which loop over the list once using _ls_p as the left, and find the _ls_q position (the start of the right sublist) using the for loop. Finally, `while (_ls_psize > 0 || (_ls_qsize > 0 && _ls_q))` is the merge part, it will merge the left and right sublist in order.

### 3.5. DL, CDL
DL and CDL are doubly-linked list and circular doubly-linked list. The overall implementation is similar to the singly linked list, but it has a pointer to the previous element. In DL, `head->prev` points to the last element(so is CDL), but the last element's next is not pointing to head (in CDL, it is). It makes APPEND in O(1) time.

### 3.6. do-while tricks in Macro:
You may notice that all the macros are using do-while(0) loop. It is a common technique in C macros to allow the macro to be used like a single statement and to avoid potential issues with semicolons.

