#ifndef HEADER_h
#define HEADER_h
#include "def.h"
#include "parser.h"

struct request check_request_line(struct request req,
                                  struct request_headers *headers,
                                  int client_fd);
#endif
