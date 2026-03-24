# HTTP Header Parsing Overview

This document explains what `parse_headers()` returns and how the supporting data structures are shaped. All of the referenced types live in `parser.h`.

## Structures Returned by `parse_headers()`

### `struct headers_status`

- `more_reads_required`: `true` if CRLF-CRLF has not been seen yet, so the caller must read more bytes; `false` once the header block is complete.
- `parsed_till_byte`: Number of bytes that have been safely consumed. While still waiting on more data it mirrors `request->size`; after the terminator is found it advances to the index immediately after the header block.
- `h`: Pointer to a heap-allocated `struct request_headers`. It stays `NULL` until headers are complete or if allocation fails.

### `struct request_headers`

- `request_line`: A `struct vector *` holding the raw request line (e.g., `"GET / HTTP/1.1"`) plus a `\0` terminator.
- `header_count`: Number of header lines following the request line.
- `headers`: Array of `struct vector *`, each entry holding one header line as a null-terminated string (no trailing CRLF).

Callers are responsible for releasing the memory with `free_request_headers()` once done.

## Example Walkthrough

Consider the buffer:

```
GET /index.html HTTP/1.1\r\n
Host: example.com\r\n
User-Agent: tiny-client\r\n\r\n
<body starts here>
```

Assume 64 bytes were read into `request`. `parse_headers(request, 64)` will:

1. Scan for the `\r\n\r\n` terminator and find it just before the body.
2. Return a `headers_status` with:
   - `more_reads_required = false`
   - `parsed_till_byte = index_of_terminator + 4` (points to the first byte of the body)
   - `h` pointing to a populated `request_headers`
3. Inside `request_headers`:
   - `request_line` stores `"GET /index.html HTTP/1.1"`
   - `header_count = 2`
   - `headers[0] = "Host: example.com"`
   - `headers[1] = "User-Agent: tiny-client"`

If the same buffer were truncated after the `User-Agent` line without the final `\r\n`, `parse_headers()` would instead return:

- `more_reads_required = true`
- `parsed_till_byte = request->size` (all bytes examined)
- `h = NULL`

## Usage Notes

- Always inspect `more_reads_required` first; do not touch `h` unless it is `false`.
- When `h` is non-NULL, you own the memory and must call `free_request_headers(h)` to avoid leaks.
- `parsed_till_byte` is useful for slicing out the header portion or for resuming parsing when additional bytes arrive.
