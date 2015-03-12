
#if ! defined(UTILS_H)
#define UTILS_H

int approxEqual(float, float, float);

typedef struct list{
 int r;
 int c;
 float val;
 int tid;
 struct list* prev;
 struct list* next;
} llist;

llist* add_list(llist* node, int r, int c, float val, int tid);
llist* get_head_list(llist* node);
llist* clear_list(llist* node);
void print_list(llist* node);

#endif
