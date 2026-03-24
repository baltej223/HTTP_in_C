#include "utils.h"
#include "def.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

bool write_to_client(int clientfd, VECTOR *data) {
  size_t size = data->size;
  char *buffer = data->data;
  int return_value = write(clientfd, &buffer, size);
  if (return_value < 0) {
    data->free_mem(data);
    perror("Can't write to client!");
  }
  data->free_mem(data);
  return true;
}

struct request create_empty_request() {
  struct request empty = {NULL, NULL, NULL, NULL, NULL};
  return empty;
}
