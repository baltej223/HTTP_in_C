#ifndef UTILS_H
#define UTILS_H
#include "vector.h"

bool write_to_client(int clientfd, VECTOR *data);
struct request create_empty_request();
#endif
