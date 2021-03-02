#define _CRT_SECURE_NO_WARNINGS
//交叉链表数据结构
#include <iostream>
#include <string>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
using namespace std;
#define MAXVERTEX 5000		//节点总数
void addCenterInitial(int);
//#define ADDR "C:\\Users\\MaZhongping\\Desktop\\Lab\\2\\pCenter-data\\data\\"	//文件路径


typedef struct Node {
	int centerVertex;											//当前节点指示的中心节点
	int beCoveredVertex;										//当前节点指定的被覆盖节点
	struct Node* nextDown;									//向下指向下一个节点
	struct Node* nextRight;									//向右指向下一个节点
}Node, * PNode;

PNode centerVertexs[MAXVERTEX]{ 0 };					//纵向，中心节点
int centerVertexCoverNum[MAXVERTEX]{ 0 };			//纵向，中心节点覆盖节点数量（未激活状态总数）
//int clientVertexCoverNum[MAXVERTEX]{ 0 };			//纵向，当前节点作为中心节点覆盖的未被覆盖的节点数量（更新）

bool isAlive[MAXVERTEX]{ 0 };								//纵向，当前节点是否激活（解结构，即X）
int bestUx;
int curUx;
int lastUx;
int beCoveredNum;											//当前被覆盖节点总数

PNode beCoveredVertex[MAXVERTEX]{ 0 };				//横向，被覆盖节点
int clientVertexBeCoveredNum[MAXVERTEX]{ 0 };	//横向，覆盖当前节点的中心节点数量
int clientVertexBeCoveredOnly[MAXVERTEX]{ -1 };	//唯一覆盖项，记录唯一覆盖该节点的节点，在add，delete中更新
int weight[MAXVERTEX]{ 0 };								//横向，被覆盖节点权重

int delta[MAXVERTEX]{ 0 };								//评估函数值
//int delta1[MAXVERTEX]{ 0 };								//评估参数复制

int tabuList[MAXVERTEX]{ 0 };							//禁忌表

int N;																//节点总数
int p;																//边总数

void LoadFile(string filePath) {	//载入文件图数据
	FILE* file = fopen(filePath.c_str(), "r");	//打开文件
	if (file == nullptr) {
		std::cout << "打开文件失败！" << endl;
		exit(-1);
	}
	PNode* curBecoveredPnode[MAXVERTEX]{ 0 };		//用于创建时维护纵向关系
	fscanf(file, "%d %d", &N, &p);

	for (int i = 0; i < N; i++) {
		curBecoveredPnode[i] = &(beCoveredVertex[i]);	//获得地址信息
	}
	PNode* curPnode;
	//读取首行（两个参数）

	int curVer = -1;
	for (int i = 0; i < N; i++) {		//数据结构横向串接
		fscanf(file, "%d", &(centerVertexCoverNum[i]));	//读入参数
		delta[i] = centerVertexCoverNum[i];
		//clientVertexCoverNum[i] = centerVertexCoverNum[i];
		curPnode = &centerVertexs[i];
		for (int j = 0; j < centerVertexCoverNum[i]; j++) {
			fscanf(file, "%d", &curVer);		//读入节点
			//建立横向串接关系
			(*curPnode) = (PNode)malloc(sizeof(Node));
			(*curPnode)->centerVertex = i;
			(*curPnode)->beCoveredVertex = curVer;
			(*curPnode)->nextDown = nullptr;
			(*curPnode)->nextRight = nullptr;

			//建立纵向关系
			(*(curBecoveredPnode[curVer])) = (*curPnode);
			//更新指示指针
			curBecoveredPnode[curVer] = &((*curPnode)->nextDown);
			curPnode = &((*curPnode)->nextRight);
		}

	}
	fclose(file);
	return;
}

/// <summary>
/// 贪心算法求初始解
/// </summary>
void InitialSolution_Greedy() {
	//按覆盖数量最多的节点依次检索并加入
	int curMax = -1, curMaxVer = -1;
	int curNum = 0;
	PNode curp1 = nullptr;
	//首先将仅由一个节点覆盖的节点的中心节点加入X，并将tt设置为无穷大
	for (int i = 0; i < N; i++) {
		curp1 = beCoveredVertex[i];
		if (curp1->nextDown == nullptr) {		//仅一个节点
			addCenterInitial(curp1->centerVertex);
			curNum++;
			tabuList[curp1->centerVertex] = 9999999;
		}
	}
	for (int i = curNum; i < p; i++) {
		curMax = -1, curMaxVer = -1;
		for (int j = 0; j < N; j++) {
			if (isAlive[j] == false && (delta[j] > curMax || (delta[j] == curMax && rand() % 2 == 0))) {	//相等时随机性更新
				curMax = delta[j];
				curMaxVer = j;
			}
		}
		//选中节点，加入X中
		addCenterInitial(curMaxVer);
	}
	return;//完成添加
}

typedef struct deltaBackup {	//delta值回溯
	int index[MAXVERTEX];
	int value[MAXVERTEX];
	int num = 0;
}deltaBackup;
void addDeltaBackupNode(deltaBackup& deltabackup, int index, int value) {
	for (int i = 0; i < deltabackup.num; i++)
		if (index == deltabackup.index[i]) return;		//存在相同值，不再添加
	deltabackup.index[deltabackup.num] = index;
	deltabackup.value[deltabackup.num] = value;
	deltabackup.num++;
}
/*void backupDelt(deltaBackup &deltabackup) {
	for (int i = 0; i < deltabackup.num; i++)
		delta[deltabackup.index[i]] = deltabackup.value[i];
	deltabackup.num = 0;		//完成回溯后，重置num值
}*/
deltaBackup deltabackup;

/*void TryToOpenCenter(int i) {		//返回被修改的下标值的原值和下标
	PNode curp2 = centerVertexs[i];
	while (curp2)
	{
		if (clientVertexBeCoveredNum[curp2->beCoveredVertex] == 1) {
			//设置唯一性覆盖，使更新的时间复杂度降低至O(1)
			addDeltaBackupNode(deltabackup, clientVertexBeCoveredOnly[curp2->beCoveredVertex], delta[clientVertexBeCoveredOnly[curp2->beCoveredVertex]]);
			delta[clientVertexBeCoveredOnly[curp2->beCoveredVertex]] -= weight[curp2->beCoveredVertex];
		}
		curp2 = curp2->nextRight;
	}
}*/


int swapi = 0, swapj = 0;
void FindPair(int iter) {
	int obj = 999999;
	//在未被覆盖的节点中任选一个v
	int v = rand() % (N - beCoveredNum);
	for (int i = 0; i < N; i++) {
		if (clientVertexBeCoveredNum[i] == 0) {
			if (v == 0) {
				v = i;
				break;
			}
			else v--;
		}
	}

	//对于每一个能覆盖v的节点i
	PNode curp1 = beCoveredVertex[v];
	PNode curp2 = nullptr;
	int i, j;
	while (curp1) {
		i = curp1->centerVertex;
		//TryToOpenCenter(i);	//尝试添加节点，更新delta值
		curp2 = centerVertexs[i];
		while (curp2)
		{
			if (clientVertexBeCoveredNum[curp2->beCoveredVertex] == 1) {
				//设置唯一性覆盖，使更新的时间复杂度降低至O(1)
				addDeltaBackupNode(deltabackup, clientVertexBeCoveredOnly[curp2->beCoveredVertex], delta[clientVertexBeCoveredOnly[curp2->beCoveredVertex]]);
				delta[clientVertexBeCoveredOnly[curp2->beCoveredVertex]] -= weight[curp2->beCoveredVertex];
			}
			curp2 = curp2->nextRight;
		}
		//TryToOpenCenter(i)结束

		//对于X中所有节点j
		for (j = 0; j < N; j++) {
			if (isAlive[j] && iter > tabuList[i] && iter > tabuList[j]) {		//禁忌表控制
				if ( delta[j] - delta[i] < obj) {
					obj = delta[j] - delta[i];
					//cout << "当前obj:" << obj << " = f("<<f<<") - delta[i]("<<delta[i]<<") + delta[j]("<<delta[j]<<")"<<endl;
					swapi = i;
					swapj = j;
				}
				else if (delta[j] - delta[i] == obj) {
					if (rand() % 2 == 0) {	//随机修改
						swapi = i; swapj = j;
					}
				}
			}
		}
		//恢复德尔塔
		//backupDelt(deltabackup);
		for (int item = 0; item < deltabackup.num; item++)
			delta[deltabackup.index[item]] = deltabackup.value[item];
		deltabackup.num = 0;		//完成回溯后，重置num值
		//backupDelt(deltabackup)结束
		curp1 = curp1->nextDown;
	}
}

void addCenterInitial(int i) {
	//横向和纵向更新
	PNode curp1 = centerVertexs[i];
	PNode curp2 = nullptr;
	isAlive[i] = true;
	while (curp1) {
		clientVertexBeCoveredNum[curp1->beCoveredVertex] += 1;		//覆盖节点的中心数量增加
		//增加前为0，表示此前并未有节点覆盖，更新其他所有覆盖该节点的delta值
		if (clientVertexBeCoveredNum[curp1->beCoveredVertex] == 1) {//从未覆盖到覆盖，所有未激活节点覆盖的未覆盖节点数-1
			//添加唯一覆盖项
			clientVertexBeCoveredOnly[curp1->beCoveredVertex] = i;	//*curp1->beCoveredVertex被i唯一性覆盖
			beCoveredNum++;
			curp2 = beCoveredVertex[curp1->beCoveredVertex];
			while (curp2) {
				if (curp2->centerVertex != i) {
					delta[curp2->centerVertex] -= weight[curp2->beCoveredVertex];	//更新delta值
				}
				curp2 = curp2->nextDown;
			}
		}
		else if (clientVertexBeCoveredNum[curp1->beCoveredVertex] == 2) {//从1到2，更新以前覆盖该节点的节点的覆盖值
			curp2 = beCoveredVertex[curp1->beCoveredVertex];
			while (curp2) {
				if (isAlive[curp2->centerVertex] && curp2->centerVertex != i) {
					delta[curp2->centerVertex] -= weight[curp2->beCoveredVertex];	//更新delta值
				}
				curp2 = curp2->nextDown;
			}
		}

		curp1 = curp1->nextRight;
	}
}

void MakeMove(int i, int j) {
	//添加节点i
	//addCenter(i);
	PNode curp1 = centerVertexs[i];
	PNode curp2 = nullptr;
	isAlive[i] = true;
	while (curp1) {
		clientVertexBeCoveredNum[curp1->beCoveredVertex] += 1;		//覆盖节点的中心数量增加
		//增加前为0，表示此前并未有节点覆盖，更新其他所有覆盖该节点的delta值
		if (clientVertexBeCoveredNum[curp1->beCoveredVertex] == 1) {//从未覆盖到覆盖，所有未激活节点覆盖的未覆盖节点数-1
			clientVertexBeCoveredOnly[curp1->beCoveredVertex] = i;	//*curp1->beCoveredVertex被i唯一性覆盖
			beCoveredNum++;
			curp2 = beCoveredVertex[curp1->beCoveredVertex];
			while (curp2) {
				if (curp2->centerVertex != i) {
					delta[curp2->centerVertex] -= weight[curp2->beCoveredVertex];	//更新delta值
				}
				curp2 = curp2->nextDown;
			}
		}
		else if (clientVertexBeCoveredNum[curp1->beCoveredVertex] == 2) {//从1到2，更新以前覆盖该节点的节点的覆盖值
			curp2 = beCoveredVertex[curp1->beCoveredVertex];
			while (curp2) {
				if (isAlive[curp2->centerVertex] && curp2->centerVertex != i) {
					delta[curp2->centerVertex] -= weight[curp2->beCoveredVertex];	//更新delta值
					break;
				}
				curp2 = curp2->nextDown;
			}
		}
		curp1 = curp1->nextRight;
	}
	//deleteCenter(j);
	curp1 = centerVertexs[j];
	isAlive[j] = false;
	//删除节点，将相关被覆盖节点中心节点-1，若原本被覆盖节点中心节点数为1，则需将相关中心节点的中心节点覆盖未覆盖节点的数量+1
	while (curp1) {
		clientVertexBeCoveredNum[curp1->beCoveredVertex] -= 1;
		if (clientVertexBeCoveredNum[curp1->beCoveredVertex] == 0) {//更新所有与当前覆盖节点相关的,从被覆盖变成未被覆盖，除i外所有Cv节点delta更新
			beCoveredNum--;
			curp2 = beCoveredVertex[curp1->beCoveredVertex];
			while (curp2) {
				if (curp2->centerVertex != j) {
					delta[curp2->centerVertex] += weight[curp2->beCoveredVertex];
				}
				curp2 = curp2->nextDown;
			}
		}
		else if (clientVertexBeCoveredNum[curp1->beCoveredVertex] == 1) {//降为1，更新相关的
			curp2 = beCoveredVertex[curp1->beCoveredVertex];
			while (curp2) {
				if (isAlive[curp2->centerVertex] == true) {
					clientVertexBeCoveredOnly[curp1->beCoveredVertex] = curp2->centerVertex;	//*curp1->beCoveredVertex被curp2->centerVertex唯一性覆盖
					delta[curp2->centerVertex] += weight[curp2->beCoveredVertex];
					break;
				}
				curp2 = curp2->nextDown;
			}
		}
		curp1 = curp1->nextRight;
	}
}

int testCalculateUx() {
	//在isAlive的条件数组下计算评估函数
	bool isCovered[MAXVERTEX]{ false };
	//逐个检测
	PNode curp1 = nullptr;
	for (int i = 0; i < N; i++) {
		if (isAlive[i] == true) {
			curp1 = centerVertexs[i];
			while (curp1) {
				isCovered[curp1->beCoveredVertex] = true;
				curp1 = curp1->nextRight;
			}
		}
	}
	int res = 0;
	for (int i = 0; i < N; i++)
		if (isCovered[i] == false) res++;
	return res;
}

int begintime;
//VWTSAlgorithm
void VWTS(int timelimit) {
	PNode curp1 = nullptr;
	beCoveredNum = 0;
	int iter = 1;
	for (int i = 0; i < N; i++)
		weight[i] = 1;
	//初始化
	InitialSolution_Greedy();
	bestUx = curUx = lastUx = N - beCoveredNum;
	std::cout << "初始化结束" << endl;
	while (curUx > 0 && clock() - begintime < timelimit) {
		FindPair(iter);
		MakeMove(swapi, swapj);
		curUx = N - beCoveredNum;
		if (curUx < bestUx)
			bestUx = curUx;
		else if (curUx >= lastUx) {
			//未被覆盖的节点，权重+1
			for (int i = 0; i < N; i++) {
				if (clientVertexBeCoveredNum[i] == 0) {
					weight[i]++;
					//更新覆盖未被覆盖的节点的权值
					curp1 = beCoveredVertex[i];
					while (curp1) {
						delta[curp1->centerVertex]++;
						curp1 = curp1->nextDown;
					}
				}
			}
		}
		tabuList[swapi] = iter + 1;
		tabuList[swapj] = iter + 1;
		lastUx = curUx;
		iter++;
		//cout << "迭代次数：" << iter - 1 << "\t当前未覆盖节点数：" << N - beCoveredNum << "\t最佳数量：" << bestUx << endl;
	}
	cout << "迭代次数：" << iter - 1 << endl;
}
/*int main() {
	std::cout << "filePath:" << endl;
	string filePath;
	cin >> filePath;
	int random = time(NULL);
	cout << "随机数种子" << random << endl;
	srand(random);
	LoadFile(ADDR + filePath);
	int time = clock();
	VWTS();
	cout << "耗时：" << clock() - time << "ms" << endl;
	std::cout << "Center Vertex:" << endl;
	for (int i = 0; i < N; i++) {
		if (isAlive[i]) std::cout << i << endl;
	}
	std::cout << "未覆盖节点数：" << testCalculateUx() << endl;
	return 0;
}*/
int main(int argv,char** args) {
	//四个参数：算例文件路径, 输出解文件路径, 运行时间上限 (单位为秒) 和随机种子 (0-65535).
	if (argv < 4) {
		cout << "参数数量不足，退出。" << endl;
		return 0;
	}
	int random;
	if(argv == 4) {//缺少最后一个参数，随机生成随机数字
		random = time(NULL)%65536;
	}
	else {
		random = atoi(args[4]);
	}
	int timelimit = atoi(args[3])*1000;		//时间上限
	cout << "随机数种子:" << random << endl;
	cout << "时间上限:" << timelimit << endl;
	srand(random);
	LoadFile(args[1]);
	begintime = clock();
	VWTS(timelimit);
	cout << "耗时：" << clock() - begintime << "ms" << endl;
	std::cout << "未覆盖节点数：" << testCalculateUx() << endl;
	if (testCalculateUx() > 0) {//未完成求解
		cout << "未完成求解，当前最优未覆盖节点数量：" << bestUx << endl;
	}
	else {
		cout << "完成求解，正在输出文件……" << endl;
		FILE* outFile = fopen(args[2], "w");		//打开文件
		if (outFile == nullptr) { cout << "打开输出文件失败！" << endl; return 0; }
		std::cout << "Center Vertex:" << endl;
		for (int i = 0; i < N; i++) {
			if (isAlive[i]) {
				std::cout << i << endl;
				fprintf(outFile, "%d ", i);
			}
		}
		fclose(outFile);
	}
	return 0;
}