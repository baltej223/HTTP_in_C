#include "vector.h"
// #include "def.h"
#include <stddef.h>
#include <string.h>

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
  v.compare_str = NULL;
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

bool compare_str(struct vector *v, char *string_to_compared_with) {
  int size_of_string = strlen(string_to_compared_with);
  if (size_of_string != v->size) {
    return false;
  }
  for (int i = 0; i < size_of_string; i++) {
    if (string_to_compared_with[i] != *(char *)v->at(v, i)) {
      return false;
    }
  }
  return true;
}

struct vector create_string_vector() {
  struct vector void_vector = create_void_vector();

  void_vector.data = calloc(void_vector.capacity, sizeof(char));
  void_vector.elem_size =
      sizeof(char); // very very important, ortherwise segfault
  void_vector.at = &at;
  void_vector.push_string = &push_string;
  void_vector.compare_str = &compare_str;
  // void vector is now a string vector
  return void_vector;
}

struct vector create_void_allocated_vector(size_t size) {
  struct vector v;
  v.capacity = size;
  v.size = 0;
  v.data = malloc(size);
  v.push = &push;
  v.free_mem = &free_mem;
  v.compare_str = NULL;
  return v;
}

struct vector buffer_to_vector(void *buffer, int buffer_size) {
  VECTOR empty_vector = create_void_allocated_vector(buffer_size);
  memcpy(empty_vector.data, buffer, buffer_size);
  empty_vector.size = buffer_size;
  return empty_vector;
}
