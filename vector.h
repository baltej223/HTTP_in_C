#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define VECTOR struct vector

struct vector {
  size_t capacity;
  size_t size;
  size_t elem_size;
  void *data;
  void (*push)(struct vector *v, void *data);
  // void (*at)(struct vector *v, size_t index) = NULL;
  void *(*at)(struct vector *v, size_t index);
  void (*free_mem)(struct vector *v);
  void *(*push_string)(struct vector *v, char *string, size_t string_size);
};

struct vector create_void_vector();
struct vector create_string_vector();

#endif
