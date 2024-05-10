#include "../uthash/include/uthash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[100];
    UT_hash_handle hh;
} user;

// Comparator functions
int id_sort_asc(user *a, user *b) {
    return (a->id - b->id);
}

int id_sort_desc(user *a, user *b) {
    return (b->id - a->id);
}

int main () {
    // users is a pointer to a hash table
    // new_user is a pointer to a user struct
    user *users = NULL, *new_user;

    // Create a new user 
    new_user = (user *)malloc(sizeof(user));
    new_user->id = 1;
    strcpy(new_user->name, "Alice");
    HASH_ADD_INT(users, id, new_user);

    // Find the user
    user *s;
    int user_id = 1;
    HASH_FIND_INT(users, &user_id, s);
    if (s == NULL) {
        printf("User %s found\n", s->name);
    }

    // Delete the user
    if (s) {
        HASH_DEL(users, s);
        free(s);
    }

    // Add some more users
    for (int i = 2; i < 10; i++) {
        new_user = (user *)malloc(sizeof(user));
        new_user->id = i;
        sprintf(new_user->name, "User %d", i);
        HASH_ADD_INT(users, id, new_user);
        printf("User %s added\n", new_user->name);
    }
        // Get the size of the hash table
    int size = HASH_COUNT(users);
    printf("Hash table size: %d\n", size);

    // Iterate over all elements in insertion order
    user *current_user, *tmp;
    printf("Iterate: \n");
    HASH_ITER(hh, users, current_user, tmp) {
        printf("\tUser ID %d: %s\n", current_user->id, current_user->name);
    }

    // Sort and delete the hash table
    HASH_SORT(users, id_sort_desc);
    printf("Sorted in desc order: \n");
    HASH_ITER(hh, users, current_user, tmp) {
        printf("\tUser ID %d: %s, delete it\n", current_user->id, current_user->name);
        HASH_DEL(users, current_user);
        free(current_user);
        size = HASH_COUNT(users);
        printf("Hash table size: %d\n", size);
    }

}


