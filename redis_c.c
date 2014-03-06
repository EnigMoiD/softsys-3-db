#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void server_disconnect(redisContext* c)
{
  redisFree(c);
}

void server_search(redisContext* c, char* search)
{
  redisReply* reply;
  redisCommand(c, "SELECT 1");
  int result_num;
  reply = (redisReply *)redisCommand(c, "SCARD", search);
  result_num = reply->integer;
  reply = (redisReply *)redisCommand(c, "KEYS %s", search);
  for (int i=0; i < result_num; i++)
  {
    printf("%s\n", reply->element[i]);
  }
}
/*
Formats the command line arguments into the form "last, first" which is how redis stores the names
first: Actors first name
last: Actors last name
*/
char *format_search(char *first, char* last)
{
  char* search = malloc(sizeof(char)*(strlen(first)+strlen(last))+3);
  search = last;
  strcat(search, ", ");
  strcat(search, first);
  printf("%s\n", search);
  return search;
}

int main(int argc, char *argv[])
{
  redisContext* c = server_connect();
  if (argc != 3) {
    printf("%s\n", "Please provide an actor name in format './redis_c George Clooney'");
    return 0;
  }
  else {
    char* search;
    search = format_search(argv[1], argv[2]);
    server_search(c,search);
  }
  redisFree(c);
  return 0;
}