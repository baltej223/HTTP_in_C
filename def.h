#ifndef DEF_H
#define DEF_H
// This file have definations of some macros
// and some imports

#include "vector.h"
#include <stdbool.h>
#include <stddef.h>

#define REQUEST struct request

struct body_struct {
  bool body_exists;
  int body_length;
  struct vector *body;
};

struct request {
  struct vector *method;
  struct vector *path;
  size_t header_count;
  struct vector **headers;
  struct body_struct body;
};

#endif
