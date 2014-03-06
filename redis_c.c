/* 
Basic Redis server search functionality

Copyright 2014 Ben Kahle and Evan Dorsky
License: Creative Commons Attribution-ShareAlike 3.0
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hiredis/hiredis.h"

redisContext *server_connect(char *ip, int port);
void server_disconnect(redisContext* c);
void server_search(redisContext* c, char* first, char* last);
char *format_search(char *first, char* last);

/*
Connects to a redis server and handles errors.
ip: server ip
port: port number
return: A redisContext pointer
*/
redisContext *server_connect(char *ip, int port)
{
  redisContext *c = redisConnect(ip, port);
  if (c != NULL && c->err) {
    printf("Error: %s\n", c->errstr);
    exit(1);
  }
  else {
    return c;
  }
}

/*
Frees a redis server connection
*/
void server_disconnect(redisContext* c)
{
  redisFree(c);
}

/*
Queries the server for all actors who are connected to the searched name
Prints to console all linked actors (First Bacon Number)
c: redisContext pointer
first: actors first name
last: actors last name 
*/
void server_search(redisContext* c, char* first, char* last)
{
  printf
  char* search = format_search(first, last);
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
Formats the first and last name into the form "last, first" which is how redis stores the names
first: Actors first name
last: Actors last name
return: formated search
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
  char* ip = "127.0.0.1";
  int port = 6379;
  redisContext* c = server_connect(ip, port);
  if (argc != 3) {
    printf("%s\n", "Please provide an actor name in format './redis_c George Clooney'");
    return 0;
  }
  else {
    server_search(c, argv[1], argv[2]);
  }
  redisFree(c);
  return 0;
}