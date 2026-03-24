#ifndef DEF_H
#define DEF_H
// This file have definations of some macros
// and some imports

#include "vector.h"
#include <stdbool.h>

#define REQUEST struct request;

struct request {
  struct vector *method;
  struct vector *path;
  struct vector *HTTP_VER;
  struct vector *headers;
  struct vector *body;
};

#endif
