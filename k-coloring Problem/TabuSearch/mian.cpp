#include<iostream>
#include "TensureSearch.h"
#include <ctime>
#define ADDR "C:\\Users\\MaZhongping\\Desktop\\Lab\\Graph Coloring����\\data\\"		//Ĭ���ļ�·��

int main(int argv, char** args) {
	string filePath;

	cout << "filename:" << endl;
	cin >> filePath;

	Graph graph;
	graph.LoadFileConstructGraph(ADDR + filePath);	//��ȡ�ļ��������ڽӾ������ݽṹ
	//graph.print();
	cout << "num of color:" << endl;
	int color_num = 0;
	cin >> color_num;
	
	TensureSearch tensure = TensureSearch(graph.getNodeNum(),color_num,time(NULL));
	tensure.TabuSearch(graph,9999999);
	tensure.printColor();
	cout<<"����ͻ����"<<tensure.TestConflict(graph)<<endl;
	getchar();
	getchar();
}