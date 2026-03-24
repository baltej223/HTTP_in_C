#ifndef PARSER_H
#define PARSER_H

#include "def.h"
#include "vector.h"

struct request_headers {
  struct vector *request_line;
  size_t header_count;
  struct vector **headers;
};

struct headers_status {
  bool more_reads_required;
  size_t parsed_till_byte;
  struct request_headers *h;
};

struct headers_status parse_headers(struct vector *request, int bytes_read);
void free_request_headers(struct request_headers *headers);

#endif
