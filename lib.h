#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "def.h"
#include "header.h"
#include "parser.h"
#include "response.h"
#include "utils.h"
#include "vector.h"

#define SERVER struct Server

// struct route_def {
//   VECTOR uri;
//   VECTOR res_body;
// };
//

/*
 * All the information related to when what to respond, will be in this struct!
 */
struct context {
  int reply_same_to_all_reqs;
  VECTOR global_response;
};

/*
 * It will have all the information required to run the server.
 * All the ports, all the routes, what to reply when, everything.
 * All the information straight here.
 */ 
struct Server {
  int PORT;
  void (*run)(struct Server *s);
  void (*port)(struct Server *s, int p); 
  struct context* cntx;
  struct body_fns* body;
};


/*
 * All the fn which will deal with request, response will be present in this struct.
 */
struct body_fns{
   void (*whatever)(struct Server *s, VECTOR *res);
};

/*
 * Declaration of Server::run
 */
void run(struct Server *s);

struct Server Create_Server();

/*
 * Declaration of the Server::body_fns::whatever
 */
void whatever(SERVER *s, VECTOR *res);
VECTOR what_to_return_to_client(SERVER *s, struct request *req);

