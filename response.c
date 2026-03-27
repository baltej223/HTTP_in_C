#include "response.h"
#include "def.h"
#include "utils.h"
#include "vector.h"
#include <stdlib.h>
#include <sys/types.h>

struct response
generate_response(struct request req) /* 2nd argument should be some other
           struct which is going to tell if the path is defined, and if yes,
           then what to return for this path*/
{
  RESPONSE res = create_empty_response();

  // pushing the server name
  VECTOR *server = malloc(sizeof(VECTOR));
  *server = create_string_vector();
  server->push_string(server, SERVER_NAME, SERVER_NAME_STRING_LENGTH);
  res.server = server;

  // pushing the date of the server
  time_t currentTime;
  time(&currentTime); // Get the current time

  VECTOR *date = malloc(sizeof(VECTOR));
  *date = create_string_vector();
  char *time_str = ctime(&currentTime);
  ssize_t time_len = strlen(time_str);
  date->push_string(date, time_str, time_len);
  res.date = date;

  if (!(req.path->compare_str(req.path, "/"))) {
    false;
  }

  // setting the content_length
  struct body_struct body = generate_body(req);
  res.content_length = body.body_length;

  // Setting the content_type
  res.content_type = body.content_type;

  // body struct
  res.body = body;

  // lets assume status to be 200 for now
  VECTOR *status = malloc(sizeof(VECTOR));
  *status = create_string_vector();
  status->push_string(status, "200 OK", 6);
  res.status = status;

  // Done and dusted
  return res;
}

struct body_struct generate_body(
    struct request req) /* 2nd argument should be some other struct which is
                           going to tell if the path is defined, and if yes,
                           then what to return for this path*/
{
  VECTOR data = create_string_vector();
  char *string = "This is the data to be sent back to the client!";
  int str_len = strlen(string);
  data.push_string(&data, string, str_len);

  struct body_struct body = create_body_struct_from_vector(data);
  VECTOR *content_t = malloc(sizeof(VECTOR));
  *content_t = create_string_vector();
  content_t->push_string(content_t, "text/plain", 10);
  body.content_type = content_t;

  return body;
}

struct vector response_to_text(struct response res) {
  VECTOR text = create_string_vector();
  text.push_string(&text, "HTTP/1.1 ", 9);

  // Now comes the status
  append_vector_to_vector(&text, res.status);
  text.push_string(&text, "\r\n", 2);

  // Now comes server-name
  text.push_string(&text, "Server: ", 8);
  append_vector_to_vector(&text, res.server);
  text.push_string(&text, "\r\n", 2);

  // Now Date
  text.push_string(&text, "Date: ", 6);
  append_vector_to_vector(&text, res.date);

  text.push_string(&text, "Content-Length: ", 16);
  char *content_length = int_to_string(res.content_length);
  int content_length_string_len = strlen(content_length);
  text.push_string(&text, content_length, content_length_string_len);
  text.push_string(&text, "\r\n", 2);
  free(content_length);

  // content type
  text.push_string(&text, "Content-Type: ", 14);
  append_vector_to_vector(&text, res.content_type);
  text.push_string(&text, "\r\n", 2);

  // Now the header ending signal
  text.push_string(&text, "\r\n", 2);

  append_vector_to_vector(&text, res.body.body);

  return text;
}
