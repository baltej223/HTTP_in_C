#ifndef DEF_H
#define DEF_H
// This file have definations of some macros
// and some imports

#include "vector.h"
#include <stdbool.h>
#include <stddef.h>

#define REQUEST struct request
#define RESPONCE struct responce

struct header_pair {
  struct vector *key;
  struct vector *value;
};

struct body_struct {
  bool body_exists;
  int body_length;
  struct vector *body;
};

struct request {
  struct vector *method;
  struct vector *path;
  size_t header_count;
  struct header_pair **headers;
  struct body_struct body;
};

struct responce {
  struct vector *status;
  struct vector *server;
  struct vector *date;
  struct vector *content_length;
  struct vector *content_type;
  struct vector *body;
};

#endif
