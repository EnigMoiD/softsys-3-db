/* 
Basic Redis server search functionality header

Copyright 2014 Ben Kahle and Evan Dorsky
License: Creative Commons Attribution-ShareAlike 3.0
*/
#ifndef REDIS_C_H
#define REDIS_C_H
#include "hiredis/hiredis.h"

redisContext *server_connect(char *ip, int port);
void server_disconnect(redisContext* c);
void server_search(redisContext* c, char* first, char* last);
char *format_search(char *first, char* last);

#endif