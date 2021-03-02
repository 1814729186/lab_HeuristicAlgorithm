#include "Graph.h"
#include <iostream>
/// ͼ���ݽṹʵ��
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

//����r����Ԫ�ص�ַ
int* const Graph::operator[](int r) {
	if (r < 0 || r >= nodeNum) return nullptr;
	return &(vetexArray[r * nodeNum]);
}

//�������
/*Graph::Graph(const Graph& graph) {
	if (this == &graph)	 return;
	//�ͷ�ԭ�ռ�
	if(vetexArray!=nullptr)
		delete[]vetexArray;
	nodeNum = graph.nodeNum;
	sumDegree = graph.sumDegree;
	vetexArray = new int[nodeNum*nodeNum];
	for (int i = 0; i < nodeNum * nodeNum; i++)
		vetexArray[i] = graph.vetexArray[i];
	return;
}*/

//�ƶ�����
/*Graph::Graph(Graph&& graph)noexcept {
	if (this == &graph)	 return;
	//�ͷ�ԭ�ռ�
	delete[]vetexArray;
	nodeNum = graph.nodeNum;
	sumDegree = graph.sumDegree;
	vetexArray = graph.vetexArray;
	graph.nodeNum = 0;
	graph.vetexArray = nullptr;
	return;
}*/

//��������д�Ⱥ�,�����ֵ����
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
//��������д���ƶ�������ֵ����
Graph& Graph::operator=(Graph&& graph)noexcept {
	if (this == &graph)	 return *this;
	//�ͷ�ԭ�ռ�
	delete[]vetexArray;
	nodeNum = graph.nodeNum;
	sumDegree = graph.sumDegree;
	vetexArray = graph.vetexArray;
	graph.nodeNum = 0;
	graph.vetexArray = nullptr;
	return *this;
}



//��ӱ���ӳɹ�����true�����򷵻�false
bool Graph::addVetex(int u, int v) {
	if(u<0||u>=nodeNum) throw"Invalid u";
	if(v<0||v>=nodeNum) throw"Invalid v";
	if (vetexArray[u * nodeNum + v] == 1 && vetexArray[v * nodeNum + u] == 1)
		return false;		//���ʧ��
	else {
		vetexArray[u * nodeNum + v] = 1;
		vetexArray[v * nodeNum + u] = 1;
		return true;
	}
}
Graph& Graph::LoadFileConstructGraph(const string filePath) {
	//���ļ��ж�ȡ��Ϣ
	char buffer[200],bufch;	//��ȡ������
	int nodeNum, degreeNum;
	FILE *filestream = fopen(filePath.c_str(),"r");
	if (filestream == nullptr) throw"Openfile Error!";
	while (true) {
		//��ȡ�׸��ַ�
		bufch = fgetc(filestream);
		if (bufch == 'c') {	//ע���У���ӡע����Ϣ
			fgets(buffer, 200, filestream);	//���ж�ȡ
			cout << buffer;
		}
		if (bufch == 'p') {	//��Ϣ�����У���ʼ��ȡͼ�ṹ��Ϣ
			fgets(buffer,6,filestream);	//����ǰ׺��ȡ
			break;
		}
	}
	//��ȡ�ļ���Ϣ
	fscanf(filestream,"%d %d\n",&nodeNum,&degreeNum);	//��ȡ��ز���
	//��ʼ����ǰͼ�ṹ
	this->nodeNum = nodeNum;
	this->sumDegree = degreeNum;
	delete[] this->vetexArray;
	this->vetexArray = new int[nodeNum * nodeNum]{0};
	//��������ȡ��ر���Ϣ��������д�ڽӾ���
	//ע�⣺���ݽṹ��ַ���㿪ʼ���ڵ����ݱ�ַ��1��ʼ��ȡ������ʱע���һ����
	int u = 0, v = 0;
	int curDegree = 0;
	while (true) {
		if((bufch=fgetc(filestream))==EOF) break;
		if (bufch != 'e') throw"error";
		fscanf(filestream, " %d %d\n", &u, &v);
		if (this->addVetex(u-1, v-1))	//��ӱ�
			curDegree += 2;
	}
	if (curDegree != degreeNum) printf("��ʶ������ʵ�ʶ�������!\n");
	return *this;	//�ƶ����忽������
}

//��ӡ����
void Graph::print() {
	cout << "NodeNum:" << nodeNum << endl;
	cout << "SumDegree:" << sumDegree << endl;
	for(int i = 0;i < this->nodeNum;i ++){
		for (int j = 0; j < this->nodeNum; j++)
			cout << this->vetexArray[i*nodeNum+j] << " ";
		cout << endl;
	}
}