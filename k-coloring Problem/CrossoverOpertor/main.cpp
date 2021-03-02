#include "CrossoverOperator.h"
#include<iostream>
#include <ctime>
#define ADDR "C:\\Users\\MaZhongping\\Desktop\\Lab\\Graph Coloring算例\\data\\"		//默认文件路径

using namespace std;
int main() {
	string filePath;

	cout << "filename:" << endl;
	cin >> filePath;

	Graph graph;
	graph.LoadFileConstructGraph(ADDR + filePath);	//读取文件，构建邻接矩阵数据结构
	cout << "num of color:" << endl;
	int color_num = 0;
	cin >> color_num;
	int* sol = new int[graph.getNodeNum()] {-1};
	int randomSeed = 0 /* time(NULL)*/;
	int begintime = clock();
	int f = HEA_Algorithm_Scheme(graph, sol, graph.getNodeNum(), color_num, P, randomSeed);
	cout << "随机数种子：" << randomSeed << endl;
	cout << "求解时间：" << clock() - begintime << endl;
	cout << "冲突数：" << f << endl;
	if(f == 0)
		for (int i = 0; i < graph.getNodeNum(); i++) {
			cout << i << " " << (int)sol[i] << endl;
		}
	//检测解结构
	cout<< "检测冲突数 " << testSolution(graph, sol) << endl;

	getchar();
	getchar();
}