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
