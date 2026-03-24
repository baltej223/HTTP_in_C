#include "header.h"
#include "def.h"
#include "parser.h"
#include "utils.h"
#include "vector.h"
#include <stdio.h>

#include <unistd.h>

struct request check_request_line(struct request_headers *headers,
                                  int client_fd) {
  VECTOR file_line =
      *(headers->request_line); // request_line is again a pointer to a vector

  VECTOR method = create_string_vector();

  struct request request_builder = create_empty_request();

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

    printf("GET request");
    fflush(stdout);
    data.push_string(&data, "Method Not allowed!", 19);
    write_to_client(client_fd, &data);
  }
}
