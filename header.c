#include "header.h"
#include "def.h"
#include "parser.h"
#include "utils.h"
#include "vector.h"
#include <stddef.h>
#include <stdio.h>

#include <stdlib.h>
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
    VECTOR file_line = *(headers->request_line);

    VECTOR *method = malloc(sizeof(VECTOR));
    if (method == NULL) {
        perror("malloc");
        return request_builder;
    }

    *method = create_string_vector();

    int iterator = 0;
    bool is_space_char = false;

    while (!is_space_char) {
        char current_char =
            *(char *)file_line.at(&file_line, iterator++);

        if (current_char == ' ') {
            is_space_char = true;
            break;
        }

        method->push(method, &current_char);
    }

    if (!(method->compare_str(method, "GET") == true ||
          method->compare_str(method, "POST") == true ||
          method->compare_str(method, "DELETE") == true)) {

        VECTOR data = create_string_vector();

        printf("Non standard request (NOT GET OR POST OR DELETE)");

        data.push_string(&data, "Method Not allowed!", 19);

        write_to_client(client_fd, &data);

        // Since we're not going to store this method,
        // free it here.
        method->free_mem(method);
        free(method);

        return request_builder;
    }

    request_builder.method = method;


    VECTOR *path = malloc(sizeof(VECTOR));
    if (path == NULL) {
        perror("malloc");

        request_builder.method->free_mem(request_builder.method);
        free(request_builder.method);
        request_builder.method = NULL;

        return request_builder;
    }

    *path = create_string_vector();

    is_space_char = false;

    while (!is_space_char) {
        char current_char =
            *(char *)file_line.at(&file_line, iterator++);

        if (current_char == ' ') {
            is_space_char = true;
            break;
        }

        path->push(path, &current_char);
    }

    if (path->size == 0 ||
        *((char *)path->at(path, 0)) != '/') {

        VECTOR bad_req_response = create_string_vector();

        bad_req_response.push_string(
            &bad_req_response,
            "Bad Request",
            11
        );

        write_to_client_and_end(client_fd, &bad_req_response);

        path->free_mem(path);
        free(path);

        return request_builder;
    }

    request_builder.path = path;

    request_builder.headers = headers->headers;
    request_builder.header_count = headers->header_count;

    return request_builder;
}

struct request extract_body(struct request req, struct vector rawrequest,
                            size_t total_bytes_read, int client_fd) {
  // check for header, Content-Length>0 if exits, that means body exists.
  // Then check for Transfer-Encoding: chunked, if it exists that means the body
  // is chuncked, and the program needs to read the the socket till signal is
  // received which is 0\r\n\r\n
  // The client_fd is active, and more content can be read which means
  // Transfer-Encoding: chunked can be handled.

  // Loop over all the headers and find Content-Length header

  /*
   * ------- Currently not going for Transfer-Encoding ------------
   */
  bool is_header_present = false;
  int content_length = 0;
  bool is_body_present = false;
  for (size_t i = 0; i < req.header_count; i++) {
    struct header_pair *pair = req.headers[i];
    if (pair == NULL) {
      continue;
    }

    if (header_key_equals(pair, "Content-Length")) {
      is_header_present = true;
      // header value is available at pair->value->data
      content_length = atoi(pair->value->data);
      break;
    }
  }
  if (is_header_present && (content_length > 0)) {
    is_body_present = true;
  }

  if (!is_body_present) {
    struct body_struct empty_body = create_empty_body_struct();
    req.body = empty_body;
    return req;
  }

  // Now if the body is present, then to read the next content_length bytes
  size_t read_till_bytes = total_bytes_read + content_length;

  VECTOR body = create_string_vector();
  for (int i = total_bytes_read; i < read_till_bytes; i++) {
    char *to_push = (char *)rawrequest.at(&rawrequest, i);
    body.push(&body, to_push);
  }
  req.body = create_body_struct_from_vector(body);
  return req;
}
