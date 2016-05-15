#include "route.h"
#include "flash.h"
#include <stdio.h>

#ifndef _WIN32
#include "lib_record.h"
#endif

void search_route(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num)
{
    Graph g;

    g.readData(topo, edge_num, demand, demand_num);

    g.findMinPathUsingBFS();

    std::vector<int> result1 = g.get_path(1);
    std::vector<int> result2 = g.get_path(2);

    if((!result1.empty()) && (!result2.empty()))
    {
        for(size_t i = 0; i < result1.size(); ++i)
            record_result(WORK_PATH, result1[i]);
        for(size_t i = 0; i < result2.size(); ++i)
            record_result(BACK_PATH, result2[i]);
    }
}
