#include "Graph.h"
#include <random>
#include <cstdio>
#include <cstdlib>
#include<ctime>
//TensureSearch算法实现

//用到的函数
//数据结构
class TensureSearch {
	int N = 0;
	int k = 0;
	int f = 0;
	int* best_Sol = nullptr;
	int best_f = 0;
	int* tabuTensure = nullptr;
	int* adjiacent_Color_Table = nullptr;
	int randomSeed = 0;
public:
	int* sol = nullptr;	//解决方案

public:
	TensureSearch(int N, int k, int randomSeed = 0);

	~TensureSearch();
	void Initialization(Graph& graph);
	void Initialization(Graph& graph, int* sol);	//使用sol数组内的数据初始化sol

	bool FindMove(int& u, int& vi, int& vj, int& delt, int iter);
	void MakeMove(int u, int vi, int vj, int delt, int iter, Graph& graph);
	int CalculateF(Graph& graph);
	void TabuSearch(Graph& graph, int maxIter,int *solution,int &f);	//禁忌搜索主方法
	void printColor();
	int TestConflict(Graph& graph);	//检测冲突数

	void TabuSearch_HEA(Graph& graph, int maxIter, int* sol, int& f);		//HEA中使用的禁忌搜索主方法


};

