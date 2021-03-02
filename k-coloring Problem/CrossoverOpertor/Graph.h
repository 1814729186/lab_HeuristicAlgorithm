#define _CRT_SECURE_NO_WARNINGS
/// ͼ���ݽṹ
///		Ϊ��ʾ�ʹ���򵥣�ʹ���ڽӾ����ʾ����ͼ����װ�������ͼ�����ɺʹ���
/// 
/// ͼ��
/// <���ݳ�Ա>
///		ͼ�ڵ�����(int)����ʾͼ�ڵ����������Ʒ����ڽӾ������
///		ͼ��ʾ�ڽӾ��󣬱�ʾͼ��
/// 
/// <������Ա>
///		���캯�� Graph(int n)������ͼ������Ϊ�ڵ����������������
///		�ƶ����� Graph(Graph& graph)	���ƶ����幹�캯��
///		����ƶ����� Graph(Graph&& graph)����������ƶ�����
///		��ӱ� addVetex(int u,int v)
///		�������� ~Graph()				
#include <string.h>
#include <iostream>
using namespace std;
class Graph {
	int nodeNum = 0;
	int* vetexArray = nullptr;	//�ڽӾ���
	int sumDegree = 0;		//�ܶ���
public:
	Graph();											//�չ���
	Graph(int nodenum, int deg);
	//Graph(const Graph& graph);				//�������
	//Graph(Graph&& a)noexcept;				//�ƶ�����
	Graph& operator=(const Graph& graph);	//�����ֵ����
	Graph& operator=(Graph&& a)noexcept;	//�ƶ���ֵ����
	int* const operator[](int r);					//�����ڽӾ���r����Ԫ��
	bool addVetex(int u, int v);					//��ӱ�
	//��������
	int getNodeNum();
	int getsumDegree();

	//��չ���ܣ���ȡ�ļ�������ͼ���ݽṹ
	Graph& LoadFileConstructGraph(string filePath);
	void print();		//��ӡ������ݣ������ã�


	~Graph();
};
