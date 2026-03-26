#ifndef HEADER_h
#define HEADER_h
#include "def.h"
#include "parser.h"

struct request check_request_line(struct request req,
                                  struct request_headers *headers,
                                  int client_fd);
struct request extract_body(struct request req, struct vector rawrequest,
                            size_t total_bytes_read, int client_fd);
#endif
