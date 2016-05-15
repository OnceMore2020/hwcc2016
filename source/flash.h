#include <list>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <climits>
#include <ctime>
#include <algorithm>
#include <queue>
#include <iostream>
#include <functional>
#include <map>
#include <string.h>
#include <unordered_map>
const int VERTEXNUM = 2000;
struct Path {
    int s;
    int d;
    std::vector<int> path;
    int weight;
    int PathID;
};
struct bfsPath
{
	std::vector<int> path;
	int outID;
	int pathWeight;
	std::vector<int> pathID;
};
// A directed graph using adjacency list representation
class Graph
{
    private:
        clock_t init_time;
        size_t edge_num;
        size_t vertex_num;
        size_t mustPassNum;
        std::list<int> *adj;
        std::list<int> *adjMustPassOne;
        //std::list<int> *adjMustPassBackOne;
		std::list<int> *adjMustPassTwo;
		//std::list<int> *adjMustPassBackTwo;
        int minPathWeightOne;
		int minPathWeightTwo;
		std::vector<std::vector<bool> > repeatPathOne;
        std::vector<int> minPathOne;
		std::vector<int> minPathTwo;
	    std::vector<int> minPathNodeID;
        //void findAllPathsUtil(int , int , bool [], int [], int &);
        std::vector<int> mustPassNodesOne;
		std::vector<int> mustPassNodesTwo;
		//std::vector<std::vector <bool> > repeatPathOne;
		int sourceID;
		int desID;
        std::vector<Path> allPathBetweenNodesOne;
		std::vector<Path> allPathBetweenNodesTwo;
        std::unordered_map<int,int> indexpathOne;
		std::unordered_map<int,int> indexpathTwo;
        bool vertices[VERTEXNUM];
    public:
        bool check_time(clock_t);
        Graph();
        size_t edgeNum();
        size_t get_vertex_num();
        size_t mustPassNodeSize();
		//void find_path();
        void addEdge(int,int,int,int);
        //void AllPaths();
        std::vector<int> get_path(int);
        void readData(char * [], int, char * [], int);
        // void findMinPathUsingDijkstra();
        void construstAdjMustPass(std::list<int>* &,std::vector<int>&,int [][VERTEXNUM]);
        void findMinPathUsingBFS();
        //std::map <std::vector < int >,int > runBfsMustPass(const std::vector<Path> &,std::list<int>*& ,std::map<int,Path>& ,std::vector<int>&,int c[][VERTEXNUM] );
	    //std::vector < int > runBfsMustPass(std::vector<Path> &,std::list<int>*& ,std::map<int,Path>& ,std::vector<int>&,int c[][VERTEXNUM] );
        void computerRepeatPath(std::vector<std::vector<bool> >&,std::vector<Path>&, std::unordered_map<int, int>&, int c[][VERTEXNUM]);
		std::vector < int > forwardBfs(std::vector <std::vector <bool> >& ,std::list<int>*&,std::unordered_map<int,int>&,std::vector<int> &,int c[][VERTEXNUM],std::vector<Path>&);
        std::vector<int> DijkstraComputerPaths(int,int,bool []);
		//void ShortestPathTwoNodes(bool [],const std::vector<int>&,std::map<int,Path>&,int [][VERTEXNUM],std::vector<Path>&);
		void ShortestPathTwoNodes(bool[], const std::vector<int>&, std::vector<Path>&);
		void addShortestPath(Path&, std::vector<int>&,int &,std::vector<Path>&);
		void findBestTwoPath(std::map <std::vector < int >,int >& ,std::map <std::vector < int >,int > &);
		std::vector <int> ComputerShortestPathOne(bool []);
		std::vector <int> ComputerShortestPathTwo(bool[]);
		void TwoOpt(std::vector<int>&, int, std::vector<Path> &, std::unordered_map<int, int>&);
};
