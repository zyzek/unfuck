#ifndef DYN_ARRAY_H
#define DYN_ARRAY_H

#define INIT_CAPACITY 50

typedef struct DynArray DynArray;

struct DynArray {
	void *array;
	void *elem;
	size_t capacity;
	size_t size;
	size_t elem_size;
};

DynArray *new_dyn_array(size_t elem_size);
void destroy_dyn_array(DynArray *arr);
bool expand(DynArray *arr);
bool shrink(DynArray *arr);
bool insert(void *elem, DynArray *arr, size_t index);
void *remove(DynArray *arr, size_t index);
bool push(void *elem, DynArray *arr);
void *pop(DynArray *arr);
#endif