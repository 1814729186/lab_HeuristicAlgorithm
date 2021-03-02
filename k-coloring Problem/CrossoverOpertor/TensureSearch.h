#include "Graph.h"
#include <random>
#include <cstdio>
#include <cstdlib>
#include<ctime>
//TensureSearch�㷨ʵ��

//�õ��ĺ���
//���ݽṹ
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
	int* sol = nullptr;	//�������

public:
	TensureSearch(int N, int k, int randomSeed = 0);

	~TensureSearch();
	void Initialization(Graph& graph);
	void Initialization(Graph& graph, int* sol);	//ʹ��sol�����ڵ����ݳ�ʼ��sol

	bool FindMove(int& u, int& vi, int& vj, int& delt, int iter);
	void MakeMove(int u, int vi, int vj, int delt, int iter, Graph& graph);
	int CalculateF(Graph& graph);
	void TabuSearch(Graph& graph, int maxIter,int *solution,int &f);	//��������������
	void printColor();
	int TestConflict(Graph& graph);	//����ͻ��

	void TabuSearch_HEA(Graph& graph, int maxIter, int* sol, int& f);		//HEA��ʹ�õĽ�������������


};

