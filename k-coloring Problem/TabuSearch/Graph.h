#define _CRT_SECURE_NO_WARNINGS
/// 图数据结构
///		为表示和处理简单，使用邻接矩阵表示无向图，封装成类进行图的生成和处理
/// 
/// 图类
/// <数据成员>
///		图节点数量(int)，表示图节点数量，控制分配邻接矩阵阶数
///		图表示邻接矩阵，表示图边
/// 
/// <函数成员>
///		构造函数 Graph(int n)，构造图，参数为节点数量（矩阵阶数）
///		移动构造 Graph(Graph& graph)	，移动语义构造函数
///		深拷贝移动构造 Graph(Graph&& graph)，深拷贝语义移动构造
///		添加边 addVetex(int u,int v)
///		析构函数 ~Graph()				
#include <string.h>
#include <iostream>
using namespace std;
class Graph {
	int nodeNum = 0;
	int* vetexArray = nullptr;	//邻接矩阵
	int sumDegree = 0;		//总度数
public:
	Graph();											//空构造
	Graph(int nodenum, int deg);
	//Graph(const Graph& graph);				//深拷贝构造
	//Graph(Graph&& a)noexcept;				//移动构造
	Graph& operator=(const Graph& graph);	//深拷贝赋值运算
	Graph& operator=(Graph&& a)noexcept;	//移动赋值运算
	int* const operator[](int r);					//返回邻接矩阵r行首元素
	bool addVetex(int u, int v);					//添加边
	//参数访问
	int getNodeNum();
	int getsumDegree();

	//扩展功能，读取文件，建立图数据结构
	Graph& LoadFileConstructGraph(string filePath);
	void print();		//打印相关数据（测试用）


	~Graph();
};
