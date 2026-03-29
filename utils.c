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
  struct body_struct empty = {false, 0, NULL, NULL};
  return empty;
}

// AI Written, thats why it sucks
struct body_struct create_body_struct_from_vector(VECTOR body) {
  struct body_struct b = {0};
  b.body_exists = true;
  b.body_length = body.size;
  b.content_type = NULL;

  struct vector *body_ptr = malloc(sizeof(struct vector));
  if (!body_ptr) {
    b.body_exists = false;
    b.body = NULL;
    return b;
  }

  *body_ptr = body;

  if (body.data == NULL) {
    body_ptr->data = NULL;
    b.body = body_ptr;
    return b;
  }

  body_ptr->data = malloc(body.capacity * body.elem_size);
  if (!body_ptr->data) {
    free(body_ptr);
    b.body_exists = false;
    b.body = NULL;
    return b;
  }

  memcpy(body_ptr->data, body.data, body.size * body.elem_size);

  b.body = body_ptr;
  return b;
}

struct response create_empty_response() {
  struct body_struct empty = {false, 0, NULL};
  struct response res = {NULL, NULL, NULL, 0, NULL, empty};
  return res;
}

char *int_to_string(int value) {
  int len = snprintf(NULL, 0, "%d", value);
  char *str = malloc(len + 1);

  if (!str)
    return NULL;

  snprintf(str, len + 1, "%d", value);
  return str;
}

void free_body_struct(struct body_struct *body) {
  if (body == NULL) {
    return;
  }
  if (body->body != NULL) {
    if (body->body->free_mem != NULL) {
      body->body->free_mem(body->body);
    }
    free(body->body);
  }
  body->body = NULL;
  body->body_exists = false;
  body->body_length = 0;
}

void free_response(struct response *res) {
  if (res == NULL)
    return;
  if (res->status != NULL) {
    res->status->free_mem(res->status);
    free(res->status);
  }
  if (res->server != NULL) {
    res->server->free_mem(res->server);
    free(res->server);
  }
  if (res->date != NULL) {
    res->date->free_mem(res->date);
    free(res->date);
  }
  if (res->content_type != NULL) {
    res->content_type->free_mem(res->content_type);
    free(res->content_type);
  }
  free_body_struct(&res->body);
}

void free_request(struct request *req) {
  if (req == NULL)
    return;
  // if (req->method != NULL) {
  //   req->method->free_mem(req->method);
  // }
  // if (req->path != NULL) {
  //   req->path->free_mem(req->path);
  // }
  if (req->headers != NULL) {
    for (size_t i = 0; i < req->header_count; i++) {
      if (req->headers[i] != NULL) {
        if (req->headers[i]->key != NULL) {
          req->headers[i]->key->free_mem(req->headers[i]->key);
          free(req->headers[i]->key);
        }
        if (req->headers[i]->value != NULL) {
          req->headers[i]->value->free_mem(req->headers[i]->value);
          free(req->headers[i]->value);
        }
        free(req->headers[i]);
      }
    }
    free(req->headers);
  }
  free_body_struct(&req->body);
}
