#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "def.h"
#include "header.h"
#include "parser.h"
#include "response.h"
#include "utils.h"
#include "vector.h"

#define PORT 3001

int main() {
  int tcp_socket = socket(AF_INET, SOCK_STREAM, 0); /* socket syscall */

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT); // htons means: host to network store
  server.sin_addr.s_addr = INADDR_ANY;

  if (!(bind(tcp_socket, (struct sockaddr *)&server, sizeof(server)) ==
        0)) /*bind syscall!*/ {
    // bind not succeeded.
    perror("Bind Error");
  }

  if (listen(tcp_socket, 100) != 0) {
    perror("TCP LISTEN ERROR");
  }

  for (int i = 0; i < 10000000;
       i++) /* Eventually will be changed to while(true) */ {
    int client_fd = accept(tcp_socket, NULL, NULL);
    if (client_fd < 0) {
      perror("Accept Error!");
    }
    char *data = "Just a TCP server sending some data!";

    // Here, read the request from the client_fd in chunckified way
    // Since a simple request is not that large, so buffer of 200 will work i
    // guess.
    // # Request Reading:
    const int READ_BUFFER_SIZE = 500;
    char buffer[READ_BUFFER_SIZE];
    VECTOR request = create_string_vector();

    ssize_t bytes_read;
    ssize_t total_bytes_read = 0;
    struct headers_status status;
    while ((bytes_read = read(client_fd, buffer, READ_BUFFER_SIZE)) > 0) {
      total_bytes_read += bytes_read;
      // I need to define another function which check if the data has ended,
      // from client side.
      // then the data will be sent to the parser to get the content-length and
      // parse headers+body or parse headers only.

      request.push_string(&request, buffer, bytes_read);
      status = parse_headers(&request, total_bytes_read);

      if (status.more_reads_required == true) {
        continue;
      } else {
        break;
      }
    }

    struct request_headers headers =
        *(status.h); // h is a pointer to all the populated headers,
                     // so I must derefrence it first.

    // Here I must parse the header, and check for insconsistencies.

    REQUEST req = create_empty_request();
    req.header_count = headers.header_count;
    req = check_request_line(req, &headers, client_fd);

    // Checking if body exists, and if yes then extracting it.
    req = extract_body(req, request, status.parsed_till_byte, client_fd);
    // Next Responce builder

    RESPONSE res = generate_response(req); // its going to give response
                                           // struct and not TEXT response

    // This is the function which is causing seg fault.

    VECTOR response_text = response_to_text(res);

    char *response_buffer = vector_to_buffer(response_text);
    ssize_t write_result =
        write(client_fd, response_buffer, response_text.size);
    if (write_result < 0) {
      perror("Write failed");
    }

    res.status->free_mem(res.status);
    free(res.status);
    res.server->free_mem(res.server);
    free(res.server);
    res.date->free_mem(res.date);
    free(res.date);
    res.content_type->free_mem(res.content_type);
    free(res.content_type);
    res.body.body->free_mem(res.body.body);
    free(res.body.body);

    response_text.free_mem(&response_text);
    free(response_buffer);

    int shut = shutdown(client_fd, SHUT_RDWR);
    fflush(stdout);
    close(client_fd);
  }
  shutdown(tcp_socket, SHUT_RDWR);
  close(tcp_socket);
}
