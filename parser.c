#include "parser.h"
#include "def.h"
#include "vector.h"

#include <stdlib.h>

static char read_char(struct vector *request, size_t index) {
  if (request == NULL || request->at == NULL) {
    return '\0';
  }

  void *value = request->at(request, index);
  if (value == NULL) {
    return '\0';
  }

  return *(char *)value;
}

static bool match_crlf(struct vector *request, size_t index) {
  if (request == NULL) {
    return false;
  }

  if (index + 1 >= request->size) {
    return false;
  }

  return read_char(request, index) == '\r' &&
         read_char(request, index + 1) == '\n';
}

static bool match_header_terminator(struct vector *request, size_t index) {
  if (index + 3 >= request->size) {
    return false;
  }

  return read_char(request, index) == '\r' &&
         read_char(request, index + 1) == '\n' &&
         read_char(request, index + 2) == '\r' &&
         read_char(request, index + 3) == '\n';
}

static size_t safe_start_index(struct vector *request, int bytes_read) {
  if (request == NULL || request->size == 0 || bytes_read <= 0) {
    return 0;
  }

  size_t recent_bytes = (size_t)bytes_read;
  if (recent_bytes >= request->size) {
    return 0;
  }

  size_t overlap = recent_bytes > 3 ? 3 : recent_bytes;
  size_t chunk_start = request->size - recent_bytes;
  return chunk_start > overlap ? chunk_start - overlap : 0;
}

static size_t count_header_lines(struct vector *request, size_t header_end) {
  size_t count = 0;
  size_t cursor = 0;

  while (cursor < header_end) {
    size_t line_end = cursor;
    while (line_end < header_end && read_char(request, line_end) != '\r') {
      line_end++;
    }

    count++;

    if (line_end + 1 >= request->size) {
      break;
    }

    cursor = line_end + 2;
  }

  return count;
}

static struct vector *copy_line(struct vector *request, size_t start,
                                size_t end) {
  struct vector *line = malloc(sizeof(struct vector));
  if (line == NULL) {
    return NULL;
  }

  *line = create_string_vector();

  for (size_t i = start; i < end && i < request->size; i++) {
    char current = read_char(request, i);
    line->push(line, &current);
  }

  char null_terminator = '\0';
  line->push(line, &null_terminator);
  return line;
}

static struct request_headers *build_headers(struct vector *request,
                                             size_t header_end) {
  size_t total_lines = count_header_lines(request, header_end);
  if (total_lines == 0) {
    return NULL;
  }

  struct request_headers *headers =
      calloc(1, sizeof(struct request_headers));
  if (headers == NULL) {
    return NULL;
  }

  if (total_lines > 1) {
    headers->header_count = total_lines - 1;
    headers->headers =
        calloc(headers->header_count, sizeof(struct vector *));
    if (headers->headers == NULL) {
      free(headers);
      return NULL;
    }
  }

  size_t cursor = 0;
  size_t line_number = 0;

  while (cursor < header_end && line_number < total_lines) {
    size_t line_end = cursor;
    while (line_end < header_end && read_char(request, line_end) != '\r') {
      line_end++;
    }

    struct vector *line = copy_line(request, cursor, line_end);
    if (line == NULL) {
      free_request_headers(headers);
      return NULL;
    }

    if (line_number == 0) {
      headers->request_line = line;
    } else if (headers->header_count > 0) {
      headers->headers[line_number - 1] = line;
    }

    line_number++;

    if (line_end + 1 >= request->size) {
      break;
    }

    cursor = line_end + 2;
  }

  return headers;
}

struct headers_status parse_headers(struct vector *request, int bytes_read) {
  struct headers_status status = {
      .more_reads_required = true,
      .parsed_till_byte = bytes_read > 0 ? (size_t)bytes_read : 0,
      .h = NULL};

  if (request == NULL || request->at == NULL || request->size == 0) {
    return status;
  }

  size_t start_index = safe_start_index(request, bytes_read);
  bool terminator_found = false;
  size_t terminator_index = request->size;

  for (size_t i = start_index; i + 3 < request->size; i++) {
    if (match_header_terminator(request, i)) {
      terminator_found = true;
      terminator_index = i;
      break;
    }
  }

  if (!terminator_found) {
    status.parsed_till_byte = request->size;
    return status;
  }

  status.more_reads_required = false;
  status.parsed_till_byte = terminator_index + 4;
  status.h = build_headers(request, terminator_index);
  return status;
}

void free_request_headers(struct request_headers *headers) {
  if (headers == NULL) {
    return;
  }

  if (headers->request_line != NULL) {
    if (headers->request_line->free_mem != NULL) {
      headers->request_line->free_mem(headers->request_line);
    }
    free(headers->request_line);
  }

  if (headers->headers != NULL) {
    for (size_t i = 0; i < headers->header_count; i++) {
      if (headers->headers[i] == NULL) {
        continue;
      }

      if (headers->headers[i]->free_mem != NULL) {
        headers->headers[i]->free_mem(headers->headers[i]);
      }
      free(headers->headers[i]);
    }

    free(headers->headers);
  }

  free(headers);
}
