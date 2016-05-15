#include "flash.h"
int Cost[VERTEXNUM][VERTEXNUM];
int Cost1[VERTEXNUM][VERTEXNUM];
std::unordered_map<int, int> LinkID;
std::unordered_map<int, int> LinkID1;
int c1[VERTEXNUM][VERTEXNUM];
int c2[VERTEXNUM][VERTEXNUM];
int table_weight[100 * VERTEXNUM] = { 0 };
//获取顶点个数
size_t Graph::get_vertex_num()
{
	size_t vert_cnt = 0;
	for (size_t i = 0; i < VERTEXNUM; ++i)
		if (vertices[i] == true)
			vert_cnt++;
	return vert_cnt;
}
void split(std::string& s, std::string delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos) {
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0) {
		ret->push_back(s.substr(last, index - last));
	}
}
Graph::Graph() :edge_num(0), vertex_num(0), mustPassNum(0)
{
	init_time = clock();
	adj = new std::list<int>[VERTEXNUM];
	minPathWeightOne = INT_MAX;
	minPathWeightTwo = INT_MAX;
}
bool Graph::check_time(clock_t crt_time)
{
	float used_time = float(crt_time - init_time) / CLOCKS_PER_SEC;
	if (used_time > 9.8)
	{
		return true;
	}
	else
		return false;
}
size_t Graph::edgeNum()
{
	return edge_num;
}

size_t Graph::mustPassNodeSize()
{
	return mustPassNodesOne.size();
}
void Graph::addEdge(int u, int v, int weight, int link_id)
{
	if (LinkID.find(u*VERTEXNUM + v) == LinkID.end())
	{
		LinkID[u*VERTEXNUM + v] = link_id;
		Cost[u][v] = weight;
		adj[u].push_back(v);
	}
	else
	{
		if (LinkID1.find(u*VERTEXNUM + v) == LinkID1.end()) {
			LinkID1[u*VERTEXNUM + v] = link_id;
			Cost1[u][v] = weight;
		}
		else {
			if (weight < Cost[u][v]) {
				Cost[u][v] = weight;
				LinkID[u*VERTEXNUM + v] = link_id;
			}
			else if (weight < Cost1[u][v]) {
				LinkID1[u*VERTEXNUM + v] = link_id;
				Cost1[u][v] = weight;
			}
		}
	}
}

std::vector<int> Graph::get_path(int path_id)
{
	if (path_id == 1)
		return this->minPathOne;
	else if (path_id == 2)
		return this->minPathTwo;
}

// 从SDK读取数据
void Graph::readData(char *topo[], int edge_n, char *demand[], int dmd_n)
{
	// 加载路径需求信息
	for (int i = 0; i < dmd_n; ++i)
	{
		std::string dmd_line(demand[i]);
		std::vector<std::string> dmd_info;
		split(dmd_line, ",", &dmd_info);
		int dmd_id = atoi(dmd_info[0].c_str());
		sourceID = (dmd_id == 1) ? atoi(dmd_info[1].c_str()) : sourceID;
		desID = (dmd_id == 1) ? atoi(dmd_info[2].c_str()) : desID;
		std::vector<std::string> must_pass;
		split(dmd_info[3], "|", &must_pass);

		if (must_pass[0].substr(0, 2) == "NA") // 处理必过点为空的情况
			continue;
		else  // 必过点不为空
			for (size_t mp_cnt = 0; mp_cnt < must_pass.size(); ++mp_cnt)
				if (dmd_id == 1)
					mustPassNodesOne.push_back(atoi(must_pass[mp_cnt].c_str()));
				else
					mustPassNodesTwo.push_back(atoi(must_pass[mp_cnt].c_str()));
	}
	// 加载图信息
	for (int topo_cnt = 0; topo_cnt < edge_n; ++topo_cnt) {
		std::string topo_line(topo[topo_cnt]);
		std::vector<std::string> line_info;
		split(topo_line, ",", &line_info);
		int link_id = atoi(line_info[0].c_str());
		int src_vert = atoi(line_info[1].c_str());
		int des_vert = atoi(line_info[2].c_str());
		int cost = atoi(line_info[3].c_str());
		addEdge(src_vert, des_vert, cost, link_id);
	}
	// 存储 edge_num
	edge_num = edge_n;
}

void Graph::construstAdjMustPass(std::list<int>* &adjMustPass, std::vector<int>& mustPassNodes, int c[][VERTEXNUM])
{
	for (size_t i = 0; i < mustPassNodes.size(); i++) {
		if (c[sourceID][mustPassNodes[i]] == INT_MAX) continue;
		else
		{
			adjMustPass[sourceID].push_back(mustPassNodes[i]);
		}
	}
	if (c[sourceID][desID] != INT_MAX) adjMustPass[sourceID].push_back(desID);
	for (size_t i = 0; i < mustPassNodes.size(); i++) {
		for (size_t j = 0; j < mustPassNodes.size(); j++) {
			if (mustPassNodes[i] != mustPassNodes[j])
			{
				if (c[mustPassNodes[i]][mustPassNodes[j]] == INT_MAX) continue;
				else {
					adjMustPass[mustPassNodes[i]].push_back(mustPassNodes[j]);
				}
			}
		}
		if (c[mustPassNodes[i]][desID] == INT_MAX) continue;
		adjMustPass[mustPassNodes[i]].push_back(desID);
	}
}
std::vector<int> Graph::DijkstraComputerPaths(const int s, const int d, bool visitedAllNodes[])
{
	visitedAllNodes[d] = false;
	visitedAllNodes[s] = false;
	std::vector<int> min_distance;
	min_distance.resize(VERTEXNUM, INT_MAX);
	min_distance[s] = 0;
	std::vector<int> previous;
	previous.resize(VERTEXNUM, -1);
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >,
		std::greater<std::pair<int, int> > > vertex_queue;
	vertex_queue.push(std::make_pair(min_distance[s], s));
	while (!vertex_queue.empty())
	{
		int dist = vertex_queue.top().first;
		int u = vertex_queue.top().second;
		vertex_queue.pop();
		if (dist>min_distance[u] || visitedAllNodes[u])
				continue;
		if (u == d)
			break;
		std::list<int>::iterator i;
		for (i = adj[u].begin(); i != adj[u].end(); ++i)
		{
			int v = *i;
			if (visitedAllNodes[v])
				continue;
			int weight = Cost[u][v];
			int distance_through_u = dist + weight;
			if (distance_through_u < min_distance[v])
			{
				min_distance[v] = distance_through_u;
				previous[v] = u;
				vertex_queue.push(std::make_pair(min_distance[v], v));
			}
		}
	}
	visitedAllNodes[d] = true;
	visitedAllNodes[s] = true;
	return previous;
}
/*
寻找必过点，原点到必过点，必过点到目标点之间的最短路径
visitedAllNodes[]:记录访问过的点
mustPassNodes：必过点
s：原点
d：目标点
indexpath：最短路径hash查找表
c[][]:记录cost
*/
void Graph::ShortestPathTwoNodes(bool visitedAllNodes[], const std::vector<int>& mustPassNodes, std::vector<Path>& allPathBetweenNodes)
{
	Path p;
	int pid = 0;
	std::vector<int> previous;
	// Initialize all vertices as not visited
	for (size_t i = 0; i < VERTEXNUM; i++)
		visitedAllNodes[i] = false;
	//initialize s,p,d nodes as visited
	for (size_t i = 0; i < mustPassNodes.size(); i++)
		visitedAllNodes[mustPassNodes[i]] = true;
	visitedAllNodes[sourceID] = true;
	visitedAllNodes[desID] = true;
	// shortest path between sourceID and mustpass nodes
	for (size_t i = 0; i < mustPassNodes.size(); i++) {
		p.s = sourceID;
		p.d = mustPassNodes[i];
		p.weight = 0;
		previous = DijkstraComputerPaths(p.s, p.d, visitedAllNodes);
		addShortestPath(p, previous, pid, allPathBetweenNodes);
	}
	p.s = sourceID;
	p.d = desID;
	p.weight = 0;
	previous = DijkstraComputerPaths(p.s, p.d, visitedAllNodes);
	addShortestPath(p, previous, pid, allPathBetweenNodes);
	//shortest path between mustpassnodes and mustpassnodes
	for (size_t i = 0; i < mustPassNodes.size(); i++)
		for (size_t j = 0; j < mustPassNodes.size(); j++)
		{
			if (i == j)
				continue;
			p.s = mustPassNodes[i];
			p.d = mustPassNodes[j];
			p.weight = 0;
			previous = DijkstraComputerPaths(p.s, p.d, visitedAllNodes);
			addShortestPath(p, previous, pid, allPathBetweenNodes);
		}
	//shortest path between mustpassnodes and desid;
	for (size_t i = 0; i < mustPassNodes.size(); i++)
	{
		p.s = mustPassNodes[i];
		p.d = desID;
		p.weight = 0;
		previous = DijkstraComputerPaths(p.s, p.d, visitedAllNodes);
		addShortestPath(p, previous, pid, allPathBetweenNodes);
	}
	/*for (int i = 0; i < allPathBetweenNodes.size(); i++) {
		for (int j = 0; j < allPathBetweenNodes[i].path.size(); j++)
			std::cout << allPathBetweenNodes[i].path[j] << " ";
		std::cout << std::endl;
	}*/
}
void Graph::addShortestPath(Path & p, std::vector<int> & previous, int &pid,
	std::vector<Path>& allPathBetweenNodes)
{
	int vertex = p.d;
	if (previous[vertex] == -1)
	{
		p.weight = INT_MAX;
		//return;
	}
	for (; previous[vertex] != -1; vertex = previous[vertex])
	{
		p.weight += Cost[previous[vertex]][vertex];
		p.path.push_back(vertex);
	}
	p.PathID = pid++;
	p.path.push_back(p.s);
	reverse(p.path.begin(), p.path.end());
	allPathBetweenNodes.push_back(p);
	p.path.clear();
}

/*
计算具有经过相同顶点的最短路径
repeatPath:记录重复矩阵
allPathBetweenNodes：所有最短路径集合
*/
void Graph::computerRepeatPath(std::vector<std::vector<bool> >& repeatPath, std::vector<Path>& allPathBetweenNodes, std::unordered_map<int, int>& indexpath, int c[][VERTEXNUM])
{
	//计算并保存有重复节点最短路径的信息
	for (std::vector<Path>::iterator itr1 = allPathBetweenNodes.begin(); itr1 != allPathBetweenNodes.end(); itr1++)
	{
		int startID = itr1->PathID;
		std::vector<int>  *p1 = &itr1->path;
		for (std::vector<Path>::iterator itr2 = itr1 + 1; itr2 != allPathBetweenNodes.end(); itr2++)
		{
			bool b = false;
			int endID = itr2->PathID;
			std::vector<int> *p2 = &itr2->path;
			for (int k = 1; k<int(p2->size()) - 1; k++) {
				if (std::find(p1->begin(), p1->end(), (*p2)[k]) != p1->end())
				{
					b = true;
					break;
				}
			}
			if (b) {
				repeatPath[startID][endID] = true;
				repeatPath[endID][startID] = true;
			}
		}
	}
	int start, end, w;
	for (size_t i = 0; i < allPathBetweenNodes.size(); i++)
	{
		start = allPathBetweenNodes[i].s;
		end = allPathBetweenNodes[i].d;
		w = allPathBetweenNodes[i].weight;
		c[start][end] = w;
		int index = start*VERTEXNUM + end;
		indexpath[index] = i;
	}
	//if (indexpath.find(434253) == indexpath.end())
		//std::cout << "Yes ";
}
bool Comparefunction(bfsPath p1, bfsPath p2)
{
	return p1.pathWeight<p2.pathWeight;
}
/*
前向BFS遍历：
repeatPath: 记录重复矩阵
adjMustPass：必过点邻接表
sourceID：原点
desID：目标点
indexpath:路径字典
mustPassNodes：必过点向量
返回值：返回找到最短路径
*/
std::vector < int > Graph::forwardBfs(std::vector <std::vector <bool> >& repeatPath, std::list<int>*& adjMustPass,
	std::unordered_map<int, int>& indexpath, std::vector<int>& mustPassNodes, int c[][VERTEXNUM],
	std::vector<Path>& allPathBetweenNodes)
{
	std::vector<bfsPath>  result;
	size_t saveNumPath = 10000;
	if (mustPassNodes.size() != 0)
		saveNumPath = 200000 / mustPassNodes.size();
	bfsPath s;
	const size_t depth = mustPassNodes.size() + 1;
	std::vector<bfsPath>  queuePathNow;
	std::vector<bfsPath>  queuePathNext;
	//std::vector<bfsPath>  queuePathTemp;
	bfsPath source;
	source.pathWeight = 0;
	source.outID = sourceID;
	source.path.push_back(sourceID);
	source.pathWeight = 0;
	queuePathNow.push_back(source);
	std::list<int>::iterator itr;
	for (size_t d = 0; d < depth; d++) {
		while (!queuePathNow.empty()) {
			s = queuePathNow.back();
			queuePathNow.pop_back();
			for (itr = adjMustPass[s.outID].begin(); itr != adjMustPass[s.outID].end(); ++itr)
			{
				if ((d == depth - 1) && (*itr != desID)) {
					continue;
				}
				if ((*itr == desID) && (d < depth - 1))
				{
					continue;
				}
				if (std::find(s.path.begin(), s.path.end(), *itr) != s.path.end()) {
					continue;
				}
				bool b = false;
				int index = s.outID*VERTEXNUM + *itr;
				int endpathID = allPathBetweenNodes[indexpath[index]].PathID;
				for (size_t i = 0; i < s.pathID.size(); i++) {
					if (repeatPath[s.pathID[i]][endpathID])
					{
						b = true; break;
					}
				}
				if (b) continue;
				int s_ID = s.outID;
				s.pathWeight = s.pathWeight + c[s_ID][*itr];
				s.outID = *itr;
				s.path.push_back(*itr);
				s.pathID.push_back(endpathID);
				queuePathNext.push_back(s);
				s.outID = s_ID;
				s.pathWeight = s.pathWeight - c[s_ID][*itr];
				s.path.pop_back();
				s.pathID.pop_back();
			}
		}
		if (queuePathNext.size() <= saveNumPath) {
			queuePathNow.clear();
			queuePathNow = queuePathNext;
			queuePathNext.clear();
		}
		else
		{
			queuePathNow.clear();
			for (int i = 0; i<100 * VERTEXNUM; i++)
				table_weight[i] = 0;
			for (std::vector<bfsPath>::iterator itr = queuePathNext.begin(); itr != queuePathNext.end(); itr++) {
				table_weight[itr->pathWeight]++;
			}
			size_t num = 0;
			int w = 0;
			for (int i = 0; i<100 * VERTEXNUM; i++) {
				num += table_weight[i];
				if (num >= saveNumPath) {
					w = i; break;
				}
			}
			for (std::vector<bfsPath>::iterator itr = queuePathNext.begin(); itr != queuePathNext.end(); itr++)
			{
				if (itr->pathWeight<w) {
					queuePathNow.push_back(*itr);
				}
				if (queuePathNow.size() == saveNumPath) {
					break;
				}
			}
			for (std::vector<bfsPath>::iterator itr = queuePathNext.begin(); itr != queuePathNext.end(); itr++)
			{
				if (queuePathNow.size() == saveNumPath) {
					break;
				}
			}
			queuePathNext.clear();
		}
	}
	std::vector < int > ret;
	if (!queuePathNow.empty()) {
		sort(queuePathNow.begin(), queuePathNow.end(), Comparefunction);
		s = queuePathNow[0];
		//std::cout << indexpath[143] << std::endl;
		//std::cout<< indexpath[]
		//std::vector<int> t=allPathBetweenNodes[indexpath[434253]].path;
		//for (int i = 0; i < t.size(); i++)
		      //std::cout << t[i]<<std::endl;
		//TwoOpt(s.path,s.pathWeight, allPathBetweenNodes,indexpath);
		for (int i = 0; i<s.path.size() - 1; i++) {
			//std::cout << s.path[i] << " ";
			int index = s.path[i] * VERTEXNUM + s.path[i + 1];
			//std::cout << index << " ";
			std::vector<int> p = allPathBetweenNodes[indexpath[index]].path;
			//for (int j = 0; j<int(p.size()) - 1; j++)
			//std::cout << p.size() << " ";
			for (int j = 0; j < p.size() - 1; j++) {
	           ret.push_back(p[j]);
			   //std::cout << p[j] << " ";
			}
			
		}
		//std::cout << std::endl;
		ret.push_back(desID);
	}
	return ret;
}
//void Graph::TwoOpt(std::vector<int>& s,int minweight,std::vector<Path>& allPathBetweenNodes, std::unordered_map<int, int>& indexpath) {
//	std::vector<int>  temp = s;
//	std::cout << "before 2opt exchange:";
//	for (int i = 0; i < s.size(); i++)
//	{
//		std::cout << s[i] << "->";
//	}
//	//std::cout << "Before 2opt exchange:";
//	std::cout << std::endl;
//	std::vector<int>::iterator itr1 = temp.begin();
//	itr1++;
//	for (; (itr1+1)!= temp.end(); itr1++) {
//		for (std::vector<int>::iterator  itr2 = itr1 + 1; (itr2+1) != temp.end(); itr2++) {
//			std::unordered_map<int, bool> map_path;
//			int weight = 0;
//			std::swap(*itr1, *itr2);
//			bool b = true;
//			for (int k = 0; k<int(temp.size()) - 1; k++) {
//				int index = temp[k] * VERTEXNUM + temp[k +1];
//				if (indexpath.find(index) == indexpath.end()) {
//					b = false;
//                    break;
//				}
//				std::vector<int> p = allPathBetweenNodes[indexpath[index]].path;
//				weight += allPathBetweenNodes[indexpath[index]].weight;
//				if (weight>minweight) { b = false; break; }
//				for (int count = 0;count<int(p.size()) - 1; count++){
//					if (map_path.find(p[count]) != map_path.end()) {
//						b = false;
//						break;
//					}
//					map_path[p[count]] = true;
//				}
//				if (!b) break;
//			}
//			map_path.clear();
//			if (!b) std::swap(*itr1, *itr2);
//			else {
//				minweight = weight;
//			}
//		}
//	}
//	s = temp;
//	std::cout << "After 2opt exchange:";
//	for (int i = 0; i < s.size(); i++)
//	{
//		std::cout << s[i]<<"->";
//	}
//}
//从两个中挑选最优的两条路径
//pathOne和pathTwo已按权重排序
//只需从这些路径找出重复边最少的两条路径
void Graph::findBestTwoPath(std::map <std::vector < int >, int >& pathOne, std::map <std::vector < int >, int > & pathTwo)
{
	if (pathOne.empty() || pathTwo.empty())
		return;
	std::cout << "pathOne size: " << pathOne.size() << std::endl;
	std::cout << "pathTwo size: " << pathTwo.size() << std::endl;
	size_t min_cnt = INT_MAX;
	std::vector<std::pair<std::vector<int>, std::vector<int> > > optima;
	std::map<std::vector<int>, int>::iterator it_one;
	std::map<std::vector<int>, int>::iterator it_two;
	for (it_one = pathOne.begin(); it_one != pathOne.end(); ++it_one)
	{
		std::vector<int> tmp_one = it_one->first;
		for (it_two = pathTwo.begin(); it_two != pathTwo.end(); ++it_two)
		{
			std::vector<int> tmp_two = it_two->first;
			size_t tmp_cnt = 0;
			for (size_t i = 0; i < tmp_two.size(); ++i)
			{
				if (std::find(tmp_one.begin(), tmp_one.end(), tmp_two[i]) != tmp_one.end())
					tmp_cnt++;
				if (tmp_cnt > min_cnt)
				{
					break;
				}
			}
			if (tmp_cnt < min_cnt)
			{
				optima.clear();
				min_cnt = tmp_cnt;
				optima.push_back(std::make_pair(tmp_one, tmp_two));
			}
			else if (tmp_cnt == min_cnt)
			{
				optima.push_back(std::make_pair(tmp_one, tmp_two));
			}
		}
	}
	size_t min_weight = INT_MAX;
	size_t min_index = 0;
	for (size_t i = 0; i < optima.size(); ++i)
	{
		size_t tmp_weight = pathOne[optima[i].first] + pathTwo[optima[i].second];
		if (tmp_weight < min_weight)
		{
			min_weight = tmp_weight;
			min_index = i;
		}
	}

	std::cout << "Optima share path count: " << min_cnt << std::endl;
	minPathOne = optima[min_index].first;
	minPathTwo = optima[min_index].second;
}
std::vector <int> Graph::ComputerShortestPathOne(bool visitedAllNodes[]) {
	std::vector <int> pathOne;
	ShortestPathTwoNodes(visitedAllNodes, mustPassNodesOne, allPathBetweenNodesOne);
	size_t num = allPathBetweenNodesOne.size() + 1;
	std::vector<std::vector<bool> > repeatPath(num, std::vector<bool>(num, false));
	computerRepeatPath(repeatPath, allPathBetweenNodesOne, indexpathOne, c1);
	adjMustPassOne = new std::list<int>[VERTEXNUM];
	construstAdjMustPass(adjMustPassOne, mustPassNodesOne, c1);
	pathOne = forwardBfs(repeatPath, adjMustPassOne, indexpathOne, mustPassNodesOne, c1, allPathBetweenNodesOne);
	delete[] adjMustPassOne;
	return pathOne;
}
std::vector <int> Graph::ComputerShortestPathTwo(bool visitedAllNodes[]) {
	std::vector <int> pathTwo;
	ShortestPathTwoNodes(visitedAllNodes, mustPassNodesTwo, allPathBetweenNodesTwo);
	size_t num = allPathBetweenNodesTwo.size() + 1;
	std::vector<std::vector<bool> > repeatPath(num, std::vector<bool>(num, false));
	computerRepeatPath(repeatPath, allPathBetweenNodesTwo, indexpathTwo, c2);
	adjMustPassTwo = new std::list<int>[VERTEXNUM];
	construstAdjMustPass(adjMustPassTwo, mustPassNodesTwo, c2);
	pathTwo = forwardBfs(repeatPath, adjMustPassTwo, indexpathTwo, mustPassNodesTwo, c2, allPathBetweenNodesTwo);
	delete[] adjMustPassTwo;
	return pathTwo;
}
void Graph::findMinPathUsingBFS()
{
	bool *visitedAllNodes = new bool[VERTEXNUM];
	//计算第一条必过点的最短路径
	std::vector <int> pathOne;
	pathOne = ComputerShortestPathOne(visitedAllNodes);
	std::vector<int> p_start;
	std::vector<int> p_end;
	std::vector<int> p_cost;
	int sumWeight = 0;
	if (pathOne.size() > 0) {
		//在第一条路径的基础上，计算第二条必过点的最短路径
		for (int i = 0; i < pathOne.size() - 1; i++) {
			int start = pathOne[i];
			int end = pathOne[i + 1];
			minPathOne.push_back(LinkID[start*VERTEXNUM + end]);
			sumWeight += Cost[start][end];
			if (LinkID1.find(start*VERTEXNUM + end) != LinkID1.end())
			{
				int temp = Cost[start][end];
				Cost[start][end] = Cost1[start][end];
				Cost1[start][end] = temp;
				temp = LinkID[start*VERTEXNUM + end];
				LinkID[start*VERTEXNUM + end] = LinkID1[start*VERTEXNUM + end];
				LinkID1[start*VERTEXNUM + end] = temp;
			}
			else {
				p_start.push_back(start);
				p_end.push_back(end);
				p_cost.push_back(Cost[start][end]);
				Cost[start][end] = 1000;
			}
		 }
	}
	std::cout << std::endl;
	minPathWeightOne = sumWeight;
	sumWeight = 0;
	int num = 0;
	std::vector< int > pathTwo;
	pathTwo = ComputerShortestPathTwo(visitedAllNodes);
	if (pathTwo.size() > 0) {
		for (int i = 0; i < pathTwo.size() - 1; i++) {
			int start = pathTwo[i];
			int end = pathTwo[i + 1];
			sumWeight += Cost[start][end];
			minPathTwo.push_back(LinkID[start*VERTEXNUM + end]);
		}
		minPathWeightTwo = sumWeight;
		sumWeight = 0;
		for (size_t i = 0; i < minPathOne.size(); i++) {
			if (std::find(minPathTwo.begin(), minPathTwo.end(), minPathOne[i]) != minPathTwo.end())
				num++;
		}
		//尝试再次寻找路径
		if (num != 0) {
			for (size_t i = 0; i < p_start.size(); i++) {
				Cost[p_start[i]][p_end[i]] = p_cost[i];
			}
			for (int i = 0; i < pathTwo.size() - 1; i++) {
				int start = pathTwo[i];
				int end = pathTwo[i + 1];
				if (LinkID1.find(start*VERTEXNUM + end) != LinkID1.end()) {
					Cost[start][end] = Cost1[start][end];
					LinkID[start*VERTEXNUM + end] = LinkID1[start*VERTEXNUM + end];
				}
				else {
					Cost[start][end] = 1000;
				}
			}
			pathOne = ComputerShortestPathOne(visitedAllNodes);
			if (!pathOne.empty()) {
				minPathOne.clear();
				for (int i = 0; i < pathOne.size() - 1; i++) {
					int start = pathOne[i];
					int end = pathOne[i + 1];
					sumWeight += Cost[start][end];
					minPathOne.push_back(LinkID[start*VERTEXNUM + end]);
				}
				minPathWeightOne = sumWeight;
			}
		}
	}
	num = 0;
	for (size_t i = 0; i < minPathOne.size(); i++) {
		if (std::find(minPathTwo.begin(), minPathTwo.end(), minPathOne[i]) != minPathTwo.end())
			num++;
	}
	std::cout << "num:" << num << std::endl;
	std::cout << "total_cost" << minPathWeightOne + minPathWeightTwo << std::endl;
	std::cout << " minPathWeightOne:" << minPathWeightOne;
	std::cout << std::endl;
	for (size_t i = 0; i < minPathTwo.size(); i++) {
		std::cout << minPathTwo[i] << "->";
	}
	std::cout << "minPathWeightTwo:" << minPathWeightTwo << std::endl;
}
