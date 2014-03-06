#include <stdio.h>
#include <stdlib.h>
#include "hiredis/hiredis.h"

redisContext *server_connect()
{
  redisContext *c = redisConnect("127.0.0.1", 6379);
  if (c != NULL && c->err) {
    printf("Error: %s\n", c->errstr);
    exit(1);
  }
  else {
    return c;
  }
}

void server_disconnect(redisContext *c)
{
  redisFree(c);
}

void server_search(redisContext *c)
{
  redisReply* reply;
  redisCommand(c, "SELECT 1");
  reply = (redisReply *)redisCommand(c, "PING");
  fprintf(stdout, "%s\n", reply->str);
}

int main(int argc, char const *argv[])
{
  redisContext *c = server_connect();
  server_search(c);
  redisFree(c);
  return 0;
}