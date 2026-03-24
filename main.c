#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "header.h"
#include "parser.h"
#include "vector.h"

#define PORT 3003

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

  for (int i = 0; i < 1; i++) /* Eventually will be changed to while(true) */ {
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

    for (int i = 0; i < request.size; i++) {
      printf("%c", *(char *)request.at(&request, i));
      fflush(stdout);
    }

    struct request_headers headers =
        *(status.h); // h is a pointer to all the populated headers,
                     // so I must derefrence it first.

    // Here I must parse the header, and check for insconsistencies.
    check_request_line(&headers, client_fd);
    //
    //
    //
    //
    ssize_t write_result = write(client_fd, data, strlen(data));
    if (write_result < 0) {
      perror("Write failed");
    }

    printf("Handling %dth request\n", i);
    int shut = shutdown(client_fd, SHUT_RDWR);
    fflush(stdout);
    close(client_fd);
  }
  shutdown(tcp_socket, SHUT_RDWR);
  close(tcp_socket);
}

/*
# Note:
- currenly the data is received by the read functio, but it blocks it, until
you manually close it, or client closes, untill then it just waits to keep
reading data, cause TCP is like a `STREAM`
- Thats why, the request is read, printed, but no response.
*/

/*
 * let pasring happen at two places, one which divides the parser and headers
 * and the request data.
 *
 *
 * */
