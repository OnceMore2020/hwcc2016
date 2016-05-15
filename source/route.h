#ifndef __ROUTE_H__
#define __ROUTE_H__

#ifndef _WIN32
#include "lib_io.h"
#endif

#ifdef _WIN32
#include "sdk.h"
#endif

void search_route(char *graph[MAX_EDGE_NUM], int edge_num, char *condition[MAX_DEMAND_NUM], int demand_num);

#endif
