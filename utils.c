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
  struct request empty = {NULL, NULL, NULL, empty_body};
  return empty;
}
