
/* The code is 
 * Copyright(c) 2018-2019 Yiqing Huang, <yqhuang@uwaterloo.ca>.
 *
 * This software may be freely redistributed under the terms of the X11 License.
 */
/**
 * @brief  stack to push/pop integer(s), API header  
 * @author yqhuang@uwaterloo.ca
 */

struct int_stack;
typedef struct frontier{
    U8 arr[100000];//png_arr[k]
    U32 index;//size of inf_array
    U32 seq;//seq number for pstack
} *frontier;

int sizeof_stack(int size);
int init_stack(struct int_stack *p, int stack_size);
struct int_stack *create_stack(int size);
void destroy_stack(struct int_stack *p);
int is_full(struct int_stack *p);
int is_empty(struct int_stack *p);
int push(struct int_stack *p, struct strip_arr item);
int pop(struct int_stack *p);
