#ifndef RESPONSE_H
#define RESPONSE_H

#include "def.h"
#include "utils.h"
#include "vector.h"
#include <time.h>

#define SERVER_NAME "C-Server"
#define SERVER_NAME_STRING_LENGTH 8

struct response generate_response(struct request req);
struct body_struct generate_body(struct request req);
struct vector response_to_text(struct response);
#endif
