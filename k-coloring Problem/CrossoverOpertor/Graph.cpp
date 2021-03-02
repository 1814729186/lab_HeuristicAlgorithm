#include "Graph.h"
#include <iostream>
/// 图数据结构实现
Graph::Graph(){}

Graph::Graph(int nodenum,int deg):nodeNum(nodenum),vetexArray(nodenum <= 0?throw"Invalid parameters":new int[nodenum*nodenum]),sumDegree(deg) {
	for (int i = 0; i < nodeNum * nodeNum; i++)
		this->vetexArray[i] = 0;
}

Graph::~Graph() {
	if(vetexArray !=nullptr)
		delete[] vetexArray;
	vetexArray = nullptr;
}

int Graph::getNodeNum() { return nodeNum; };
int Graph::getsumDegree() { return sumDegree; };

//返回r行首元素地址
int* const Graph::operator[](int r) {
	if (r < 0 || r >= nodeNum) return nullptr;
	return &(vetexArray[r * nodeNum]);
}

//深拷贝构造
/*Graph::Graph(const Graph& graph) {
	if (this == &graph)	 return;
	//释放原空间
	if(vetexArray!=nullptr)
		delete[]vetexArray;
	nodeNum = graph.nodeNum;
	sumDegree = graph.sumDegree;
	vetexArray = new int[nodeNum*nodeNum];
	for (int i = 0; i < nodeNum * nodeNum; i++)
		vetexArray[i] = graph.vetexArray[i];
	return;
}*/

//移动构造
/*Graph::Graph(Graph&& graph)noexcept {
	if (this == &graph)	 return;
	//释放原空间
	delete[]vetexArray;
	nodeNum = graph.nodeNum;
	sumDegree = graph.sumDegree;
	vetexArray = graph.vetexArray;
	graph.nodeNum = 0;
	graph.vetexArray = nullptr;
	return;
}*/

//操作符重写等号,深拷贝赋值运算
Graph& Graph::operator=(const Graph& graph) {
	if (this == &graph)	 return *this;
	delete[] vetexArray;
	nodeNum = graph.nodeNum;
	sumDegree = graph.sumDegree;
	vetexArray = new int[nodeNum * nodeNum];
	for (int i = 0; i < nodeNum * nodeNum; i++)
		vetexArray[i] = graph.vetexArray[i];
	return *this;
}
//操作符重写，移动拷贝赋值运算
Graph& Graph::operator=(Graph&& graph)noexcept {
	if (this == &graph)	 return *this;
	//释放原空间
	delete[]vetexArray;
	nodeNum = graph.nodeNum;
	sumDegree = graph.sumDegree;
	vetexArray = graph.vetexArray;
	graph.nodeNum = 0;
	graph.vetexArray = nullptr;
	return *this;
}



//添加边添加成功返回true，否则返回false
bool Graph::addVetex(int u, int v) {
	if(u<0||u>=nodeNum) throw"Invalid u";
	if(v<0||v>=nodeNum) throw"Invalid v";
	if (vetexArray[u * nodeNum + v] == 1 && vetexArray[v * nodeNum + u] == 1)
		return false;		//添加失败
	else {
		vetexArray[u * nodeNum + v] = 1;
		vetexArray[v * nodeNum + u] = 1;
		return true;
	}
}
Graph& Graph::LoadFileConstructGraph(const string filePath) {
	//从文件中读取信息
	char buffer[200],bufch;	//读取缓冲区
	int nodeNum, degreeNum;
	FILE *filestream = fopen(filePath.c_str(),"r");
	if (filestream == nullptr) throw"Openfile Error!";
	while (true) {
		//读取首个字符
		bufch = fgetc(filestream);
		if (bufch == 'c') {	//注释行，打印注释信息
			fgets(buffer, 200, filestream);	//按行读取
			cout << buffer;
		}
		if (bufch == 'p') {	//信息描述行，开始读取图结构信息
			fgets(buffer,6,filestream);	//跳过前缀读取
			break;
		}
	}
	//获取文件信息
	fscanf(filestream,"%d %d\n",&nodeNum,&degreeNum);	//读取相关参数
	//初始化当前图结构
	this->nodeNum = nodeNum;
	this->sumDegree = degreeNum;
	delete[] this->vetexArray;
	this->vetexArray = new int[nodeNum * nodeNum]{0};
	//接下来读取相关边信息并创建填写邻接矩阵
	//注意：数据结构编址从零开始，节点数据编址从1开始，取用数据时注意减一操作
	int u = 0, v = 0;
	int curDegree = 0;
	while (true) {
		if((bufch=fgetc(filestream))==EOF) break;
		if (bufch != 'e') throw"error";
		fscanf(filestream, " %d %d\n", &u, &v);
		if (this->addVetex(u-1, v-1))	//添加边
			curDegree += 2;
	}
	if (curDegree != degreeNum) printf("标识度数与实际度数不符!\n");
	return *this;	//移动语义拷贝返回
}

//打印函数
void Graph::print() {
	cout << "NodeNum:" << nodeNum << endl;
	cout << "SumDegree:" << sumDegree << endl;
	for(int i = 0;i < this->nodeNum;i ++){
		for (int j = 0; j < this->nodeNum; j++)
			cout << this->vetexArray[i*nodeNum+j] << " ";
		cout << endl;
	}
}