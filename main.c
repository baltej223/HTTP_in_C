#include "lib.h"

int main(){
  SERVER s = Create_Server();
  s.port(&s, 3000);
  VECTOR reply = create_string_vector();
  reply.push_string(&reply, "HYE", 3);
  s.body->whatever(&s, &reply);
  s.run(&s);
  return 0;
}
