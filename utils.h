#ifndef UTILS_H
#define UTILS_H

#include "def.h"
#include "vector.h"
#include <stddef.h>

bool write_to_client(int clientfd, VECTOR *data);
bool write_to_client_and_end(int clientfd, VECTOR *data);
struct request create_empty_request();
void trim_whitespace(const char *buffer, size_t len, size_t *start_index,
                     size_t *end_index);

struct body_struct create_empty_body_struct();
struct body_struct create_body_struct_from_vector(VECTOR body);
void free_body_struct(struct body_struct *body);

struct response create_empty_response();
char *int_to_string(int value);
#endif
