#include<iostream>
#include "TensureSearch.h"
#include <ctime>
#define ADDR "C:\\Users\\MaZhongping\\Desktop\\Lab\\Graph Coloring算例\\data\\"		//默认文件路径

int main(int argv, char** args) {
	string filePath;

	cout << "filename:" << endl;
	cin >> filePath;

	Graph graph;
	graph.LoadFileConstructGraph(ADDR + filePath);	//读取文件，构建邻接矩阵数据结构
	//graph.print();
	cout << "num of color:" << endl;
	int color_num = 0;
	cin >> color_num;
	
	TensureSearch tensure = TensureSearch(graph.getNodeNum(),color_num,time(NULL));
	tensure.TabuSearch(graph,9999999);
	tensure.printColor();
	cout<<"检测冲突数："<<tensure.TestConflict(graph)<<endl;
	getchar();
	getchar();
}