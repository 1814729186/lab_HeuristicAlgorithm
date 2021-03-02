#include "CrossoverOperator.h"
#include<iostream>
#include <ctime>
#define ADDR "C:\\Users\\MaZhongping\\Desktop\\Lab\\Graph Coloring����\\data\\"		//Ĭ���ļ�·��

using namespace std;
int main() {
	string filePath;

	cout << "filename:" << endl;
	cin >> filePath;

	Graph graph;
	graph.LoadFileConstructGraph(ADDR + filePath);	//��ȡ�ļ��������ڽӾ������ݽṹ
	cout << "num of color:" << endl;
	int color_num = 0;
	cin >> color_num;
	int* sol = new int[graph.getNodeNum()] {-1};
	int randomSeed = 0 /* time(NULL)*/;
	int begintime = clock();
	int f = HEA_Algorithm_Scheme(graph, sol, graph.getNodeNum(), color_num, P, randomSeed);
	cout << "��������ӣ�" << randomSeed << endl;
	cout << "���ʱ�䣺" << clock() - begintime << endl;
	cout << "��ͻ����" << f << endl;
	if(f == 0)
		for (int i = 0; i < graph.getNodeNum(); i++) {
			cout << i << " " << (int)sol[i] << endl;
		}
	//����ṹ
	cout<< "����ͻ�� " << testSolution(graph, sol) << endl;

	getchar();
	getchar();
}