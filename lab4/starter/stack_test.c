// /**
//  * @brief  A shared memory that makes use of C struct 
//  * @author yqhuang@uwaterloo.ca
//  */


// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <errno.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <sys/shm.h>
// #include <semaphore.h>
// #include "stack.h"

// #define STACK_SIZE 3
// void push_all(struct int_stack *p);
// void pop_all(struct int_stack *p);
// void test_local();
// void test_shm();

// /* pushing STACK_SIZE items to the stack */
// void push_all(struct int_stack *p)
// {
//     push(p, "hey");
    
// }

// /* pop STACK_SIZE items off the stack */
// void pop_all(struct int_stack *p)
// {
//     int item;
//     pop(p, &item);
//     printf("Item: %d", item);

// }

// void test_local()
// {
//     int i;
//     struct int_stack *pstack;

//     if ( (pstack = create_stack(STACK_SIZE)) == NULL) {
//         fprintf(stderr, "Failed to create a new stack, abort...\n");
//         abort();
//     };

//     push_all(pstack);
//     pop_all(pstack);
//     destroy_stack(pstack);
// }

// // int main()
// // {
// //     test_local();
// //     return 0;

// // }