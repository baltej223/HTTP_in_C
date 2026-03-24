#include "vector.h"
#include <stddef.h>

void push(struct vector *v, void *data) {
  // v is 'this' vector
  if (v->capacity == v->size) {
    v->capacity *= 2;

    void *temp = realloc(v->data, v->capacity * v->elem_size);
    if (temp == NULL) {
      // dangerous here.
      push(v, data);
    } else {
      v->data = temp;
    }
  }
  memcpy((char *)v->data + (v->size * v->elem_size), data, v->elem_size);
  v->size++;
}

void free_mem(struct vector *v) { free(v->data); }

struct vector create_void_vector() {
  struct vector v;
  v.capacity = 1;
  v.size = 0;
  // No v.data defination here.
  v.push = &push;
  v.free_mem = &free_mem;
  v.data = NULL;
  return v;
}

void *at(struct vector *v, size_t index) {
  if (index >= v->size) {
    return NULL;
  }

  return (char *)v->data + (index * v->elem_size);
  // Here is a doubt: “Treat memory as raw bytes so I can do pointer arithmetic
  // safely”
}

void *push_string(struct vector *v, char *string, size_t size_of_string) {
  for (int i = 0; i < size_of_string; i++) {
    v->push(v, &string[i]);
  }
}

struct vector create_string_vector() {
  struct vector void_vector = create_void_vector();

  void_vector.data = calloc(void_vector.capacity, sizeof(char));
  void_vector.elem_size =
      sizeof(char); // very very important, ortherwise segfault
  void_vector.at = &at;
  void_vector.push_string = &push_string;
  // void vector is now a string vector
  return void_vector;
}
