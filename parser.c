#include "parser.h"
#include "def.h"
#include "utils.h"
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

static void free_header_pair(struct header_pair *pair);

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

static bool copy_slice_into_vector(struct vector *dest, const char *src,
                                   size_t len) {
  if (dest == NULL || src == NULL) {
    return false;
  }

  for (size_t i = 0; i < len; i++) {
    char ch = src[i];
    dest->push(dest, &ch);
  }

  char null_terminator = '\0';
  dest->push(dest, &null_terminator);
  return true;
}

static struct header_pair *parse_header_line(struct vector *line) {
  if (line == NULL || line->data == NULL) {
    return NULL;
  }

  struct header_pair *pair = calloc(1, sizeof(struct header_pair));
  if (pair == NULL) {
    return NULL;
  }

  struct vector *key_vec = malloc(sizeof(struct vector));
  struct vector *value_vec = malloc(sizeof(struct vector));
  if (key_vec == NULL || value_vec == NULL) {
    free(key_vec);
    free(value_vec);
    free(pair);
    return NULL;
  }

  *key_vec = create_string_vector();
  *value_vec = create_string_vector();
  pair->key = key_vec;
  pair->value = value_vec;

  char *raw_line = (char *)line->data;
  size_t line_len = line->size;
  if (line_len > 0 && raw_line[line_len - 1] == '\0') {
    line_len--;
  }

  size_t colon_index = (size_t)-1;
  for (size_t i = 0; i < line_len; i++) {
    if (raw_line[i] == ':') {
      colon_index = i;
      break;
    }
  }

  const char *key_buffer = raw_line;
  size_t key_len = colon_index == (size_t)-1 ? line_len : colon_index;
  size_t key_start = 0;
  size_t key_end = key_len;
  trim_whitespace(key_buffer, key_len, &key_start, &key_end);

  const char *value_buffer = colon_index == (size_t)-1
                                 ? raw_line + line_len
                                 : raw_line + colon_index + 1;
  size_t value_len =
      colon_index == (size_t)-1 ? 0 : line_len - (colon_index + 1);
  size_t value_start = 0;
  size_t value_end = value_len;
  trim_whitespace(value_buffer, value_len, &value_start, &value_end);

  const char *trimmed_key = key_buffer + key_start;
  size_t trimmed_key_len = key_end > key_start ? key_end - key_start : 0;
  const char *trimmed_value = value_buffer + value_start;
  size_t trimmed_value_len =
      value_end > value_start ? value_end - value_start : 0;

  if (!copy_slice_into_vector(pair->key, trimmed_key, trimmed_key_len) ||
      !copy_slice_into_vector(pair->value, trimmed_value, trimmed_value_len)) {
    free_header_pair(pair);
    return NULL;
  }

  return pair;
}

static struct request_headers *build_headers(struct vector *request,
                                             size_t header_end) {
  size_t total_lines = count_header_lines(request, header_end);
  if (total_lines == 0) {
    return NULL;
  }

  struct request_headers *headers = calloc(1, sizeof(struct request_headers));
  if (headers == NULL) {
    return NULL;
  }

  if (total_lines > 1) {
    headers->header_count = total_lines - 1;
    headers->headers =
        calloc(headers->header_count, sizeof(struct header_pair *));
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
      struct header_pair *pair = parse_header_line(line);
      if (pair == NULL) {
        if (line->free_mem != NULL) {
          line->free_mem(line);
        }
        free(line);
        free_request_headers(headers);
        return NULL;
      }

      headers->headers[line_number - 1] = pair;

      if (line->free_mem != NULL) {
        line->free_mem(line);
      }
      free(line);
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
  struct headers_status status = {.more_reads_required = true,
                                  .parsed_till_byte =
                                      bytes_read > 0 ? (size_t)bytes_read : 0,
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

static void free_header_pair(struct header_pair *pair) {
  if (pair == NULL) {
    return;
  }

  if (pair->key != NULL) {
    if (pair->key->free_mem != NULL) {
      pair->key->free_mem(pair->key);
    }
    free(pair->key);
  }

  if (pair->value != NULL) {
    if (pair->value->free_mem != NULL) {
      pair->value->free_mem(pair->value);
    }
    free(pair->value);
  }

  free(pair);
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
      free_header_pair(headers->headers[i]);
    }

    free(headers->headers);
  }

  free(headers);
}
