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
typedef struct char_stack{
    char char_list[256];
}*char_stack;
typedef unsigned char U8;
typedef unsigned int  U32;
typedef unsigned long int U64;
typedef struct front {
  char to_visit[10000][256];
  int size;
} FRONT;

int sizeof_shm_stack(int size);
int init_shm_stack(struct int_stack *p, int stack_size);
struct int_stack *create_stack(int size);
void destroy_stack(struct int_stack *p);
int is_full(struct int_stack *p);
int is_empty(struct int_stack *p);
int push(struct int_stack *p, struct char_stack item);
int pop(struct int_stack *p);
