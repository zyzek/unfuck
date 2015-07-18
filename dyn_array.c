#include <stdlib.h>
#include "dyn_array.h"

DynArray *new_dyn_array(size_t elem_size) {
	DynArray *new_arr = (DynArray *)malloc(sizeof(DynArray));

	if (!new_arr) {
		return NULL;
	}

	new_arr->array = calloc(INIT_CAPACITY, elem_size);
	new_arr->elem = malloc(elem_size);
	
	if (!new_arr->array || !new_arr->elem) {
		if (new_arr->array) free(new_arr->array);
		if (new_arr->elem) free(new_arr->elem);

		free(new_arr);
		return NULL;
	}
	
	new_arr->capacity = INIT_CAPACITY;
	new_arr->size = 0;
	new_arr->elem_size = elem_size;

	return new_arr;
}

void destroy_dyn_array(DynArray *arr) {
	free(arr->array);
	free(arr->elem);
	free(arr);
}

bool expand(DynArray *arr) {
	void *new_array = realloc(arr->array, arr->capacity * 2);

	if (new_array) {
		arr->capacity *= 2;
		arr->array = new_array;

		return true;
	}
	else {
		return false;
	}
}

bool shrink(DynArray *arr) {
	if (arr->capacity <= 2) return false;

	void *new_array = realloc(arr->array, arr->capacity / 2);

	if (new_array) {
		arr->capacity /= 2;
		arr->array = new_array;

		return true;
	}
	else {
		return false;
	}
}

bool insert(void *elem, DynArray *arr, size_t index) {
	if (index > arr->size) {
		return false;
	}
	
	if (arr->size == arr->capacity) {
		bool success = expand(arr);

		if (!success) return false;
	}

	size_t e_s = arr->elem_size;
	char *a = (char*)arr->array;
	for (int i = arr->size; i > index; --i) {
		memcpy(a + i*e_s, a + (i - 1)*e_s, e_s);
	}

	memcpy(a + index*e_s, elem, e_s);

	++(arr->size);

	return true;
}

void *remove(DynArray *arr, size_t index) {
	if (index >= arr->size) {
		return NULL;
	}

	size_t e_s = arr->elem_size;
	char*a = (char*)arr->array;

	memcpy(arr->elem, a + index*e_s, e_s);

	for (int i = index; i < arr->size; ++i) {
		memcpy(a + i*e_s, a + (i + 1)*e_s, e_s);
	}

	--(arr->size);

	if ((arr->size > 1) && (arr->size < arr->capacity / 3)) {
		shrink(arr);
	}

	return arr->elem;
}

bool push(void *elem, DynArray *arr) {
	return insert(elem, arr, arr->size);
}
void *pop(DynArray *arr) {
	return remove(arr, arr->size - 1);
}