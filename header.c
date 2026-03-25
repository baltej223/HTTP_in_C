#include "header.h"
#include "def.h"
#include "parser.h"
#include "utils.h"
#include "vector.h"
#include <stdio.h>

#include <string.h>
#include <strings.h>
#include <unistd.h>

bool header_key_equals(struct header_pair *pair, const char *key) {
  if (pair == NULL || pair->key == NULL || pair->key->data == NULL ||
      key == NULL) {
    return false;
  }
  return strcasecmp((char *)pair->key->data, key) == 0;
}

struct request check_request_line(struct request request_builder,
                                  struct request_headers *headers,
                                  int client_fd) {
  VECTOR file_line =
      *(headers->request_line); // request_line is again a pointer to a vector

  VECTOR method = create_string_vector();

  int iterator = 0;
  bool is_space_char = false;
  while (!is_space_char) {
    char current_char = *(char *)file_line.at(&file_line, iterator++);
    if (current_char == ' ') {
      is_space_char = true;
      break;
    }
    method.push(&method, (char *)&current_char);
  }

  if (!(method.compare_str(&method, "GET") == true ||
        method.compare_str(&method, "POST") == true ||
        method.compare_str(&method, "DELETE") == true)) {
    VECTOR data = create_string_vector();

    printf("Non strandard request (NOT GET OR PUT OR DEL)");
    fflush(stdout);
    data.push_string(&data, "Method Not allowed!", 19);
    write_to_client(client_fd, &data);
  }
  request_builder.method = &method;
  // Reqest method id validated!
  // Next is path

  // The iterator is already pointing to character which is already read, so we
  // resume from there only.

  VECTOR path = create_string_vector();
  is_space_char = false;
  while (!is_space_char) {
    char current_char = *(char *)file_line.at(&file_line, iterator++);
    if (current_char == ' ') {
      is_space_char = true;
      break;
    }
    path.push(&path, (char *)&current_char);
  }
  if (!(*((char *)path.at(&path, 0)) == '/')) /* Check One */ {
    VECTOR bad_req_responce = create_string_vector();
    bad_req_responce.push_string(&bad_req_responce, "Bad Request", 11);
    write_to_client_and_end(client_fd, &bad_req_responce);
  }
  request_builder.path = &path;
  // Path is also verified.

  // Next is to put all the headers in request_builder.headers;
  request_builder.headers = headers->headers;
  request_builder.header_count = headers->header_count;

  // Verification of the first line is done! Lets return
  return request_builder;
}

struct request extract_body(struct request req) {
  // check for header, Content-Length>0 if exits, that means body exists.
  // Then check for Transfer-Encoding: chunked, if it exists that means the body
  // is chuncked, and the program needs to read the the socket till signal is
  // received which is 0\r\n\r\n
  // The client_fd is active, and more content can be read which means
  // Transfer-Encoding: chunked can be handled.

  // Loop over all the headers and find Content-Length header
  bool is_header_present = false;
  for (size_t i = 0; i < req.header_count; i++) {
    struct header_pair *pair = req.headers[i];
    if (pair == NULL) {
      continue;
    }

    if (header_key_equals(pair, "Content-Length")) {
      is_header_present = true;
      // header value is available at pair->value->data
      // TODO: parse value and populate req.body.
    }
  }

  (void)is_header_present;
  return req;
}
