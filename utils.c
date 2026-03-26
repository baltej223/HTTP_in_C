#include "utils.h"
#include "def.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

bool write_to_client(int clientfd, VECTOR *data) {
  size_t size = data->size;
  char *buffer = data->data;
  int return_value = write(clientfd, buffer, size);
  if (return_value < 0) {
    data->free_mem(data);
    perror("Can't write to client!");
  }
  data->free_mem(data);
  return true;
}

bool write_to_client_and_end(int clientfd, VECTOR *data) {
  write_to_client(clientfd, data);
  int shut = shutdown(clientfd, SHUT_RDWR);
  close(clientfd);
  /*
    //    * Just realised one thing, which is that the original (outer) loop,
    must
    //    be
    //    * `continued` (using continue statement) whenever I call
    //    * write_to_client_and_end(), Cause otherwise, the other outer write
    //    * function is still going to write to client;
    //    */

  return true;
}

struct request create_empty_request() {
  struct body_struct empty_body = {false, 0, NULL};
  struct request empty = {NULL, NULL, 0, NULL, empty_body};
  return empty;
}

void trim_whitespace(const char *buffer, size_t len, size_t *start_index,
                     size_t *end_index) {
  if (buffer == NULL) {
    if (start_index != NULL) {
      *start_index = 0;
    }
    if (end_index != NULL) {
      *end_index = 0;
    }
    return;
  }
  size_t start = 0;
  size_t end = len;

  while (start < len && (buffer[start] == ' ' || buffer[start] == '\t')) {
    start++;
  }
  while (end > start && (buffer[end - 1] == ' ' || buffer[end - 1] == '\t')) {
    end--;
  }
  if (start_index != NULL) {
    *start_index = start;
  }
  if (end_index != NULL) {
    *end_index = end;
  }
}

struct body_struct create_empty_body_struct() {
  struct body_struct empty = {false, 0, NULL};
  return empty;
}

struct body_struct create_body_struct_from_vector(VECTOR body) {
  struct body_struct b = {.body_exists = true};
  int size = body.size;
  b.body_length = size;

  struct vector *body_ptr = malloc(sizeof(struct vector));
  if (body_ptr == NULL) {
    b.body_exists = false;
    b.body = NULL;
    return b;
  }
  *body_ptr = body;
  b.body = body_ptr;
  return b;
}

// void free_body_struct(struct body_struct *body) {
//   if (body == NULL) {
//     return;
//   }
//   if (body->body != NULL) {
//     if (body->body->free_mem != NULL) {
//       body->body->free_mem(body->body);
//     }
//     free(body->body);
//   }
//   body->body = NULL;
//   body->body_exists = false;
//   body->body_length = 0;
// }
