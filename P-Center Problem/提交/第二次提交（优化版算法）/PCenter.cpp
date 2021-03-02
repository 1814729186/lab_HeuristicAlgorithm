#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
using namespace std;
//使用递归邻域搜索，仅最优更新


/// PCenter问题，提高访问速率，可变长数组数据结构模拟集合
#define MAXNODENUM 5000		//最大节点数量

#define ADDNODENUM 20		//单次添加可变长数组大小
//#define ADDR "C:\\Users\\MaZhongping\\Desktop\\Lab\\2\\pCenter-data\\data\\"
bool addCenter(int node,int iter);
int testCalculateUx();


int nodeBeCoveredNum[MAXNODENUM]{ 0 };		//覆盖该节点的中心的激活数量
int beCoverdeOnly[MAXNODENUM]{ 0 };			//唯一性覆盖

bool isAlive[MAXNODENUM]{ false };	//当前节点是否激活
int coverNum[MAXNODENUM]{ 0 };	//节点覆盖元素数量
int N, p;										//基本数据
int weight[MAXNODENUM]{ 0 };		//节点权值
int delta[MAXNODENUM]{ 0 };			//节点δ
int tabuList[MAXNODENUM]{ 0 };		//禁忌表
int iter = 0;

int addCenterIter[MAXNODENUM]{ 0 };		//集合添加时间戳，用于跳出局部最优

int Ux;
int bestUx;
int lastUx;
class Node {//覆盖或被覆盖关系描述
public:
	int* coverNode;		//当前覆盖或被覆盖的节点数组
	int curNum;			//当前储存元素数量
	int curMaxNum;	//当前数据结构储存最多元素数量
	void addCoverNode(int i);	//添加覆盖节点
	bool deleteCoverNode(int elem);	//删除覆盖关系节点
	bool containElem(int node);			//查询包含关系
	Node();						//构造函数
	~Node();					//析构函数
};
Node::Node() {	//构造函数
	coverNode = new int[ADDNODENUM];
	curMaxNum = ADDNODENUM;
	curNum = 0;
}
Node::~Node() {
	delete[] coverNode;
	coverNode = nullptr;
	curMaxNum = 0;
	curNum = 0;
}

//添加覆盖节点
void Node::addCoverNode(int node) {
	if (curNum == curMaxNum) {	//达到最大元素数量，分配新的数据结构，复制原有数据
		int* newArray = new int[curMaxNum + ADDNODENUM];
		for (int i = 0; i < curNum; i++) {
			newArray[i] = coverNode[i];
		}
		delete[] coverNode;
		coverNode = newArray;
		curMaxNum += ADDNODENUM;
	}
	coverNode[curNum++] = node;
}
bool Node::deleteCoverNode(int node) {
	//删除覆盖节点关系
	int index = -1;
	for (int i = 0; i < curNum; i++) {
		if (coverNode[i] == node) {
			index = i;
			break;
		}
	}
	if (index == -1)	return false;
	for (int i = index; i < curNum; i++) {
		coverNode[i] = coverNode[i + 1];
	}
	curNum--;
	return true;
}
bool Node::containElem(int node) {
	for (int i = 0; i < curNum; i++) {
		if (coverNode[i] == node) return true;
	}
	return false;
}

int cmpSet(Node& V1, Node& V2);


class SolutionSpace {	//模拟解空间的类
public:
	int* elem;	//解空间 
	int curNum;		//解空间当前元素数量
	int max;			//解空间最大元素数量

	void addElem(int node);		//添加元素进入解空间
	void deleteElemByIndex(int index);	//从解空间删除元素,使用下标作为删除依据
	void deleteElemByElem(int index);	//从解空间删除元素,使用下标作为删除依据
	SolutionSpace();			//构造函数，参数是p
	void Initial(int p);				//初始化，使用初始化函数而不是构造函数进行初始化，保证全局变量的设置
	~SolutionSpace();				//析构函数
};
//构造函数
SolutionSpace::SolutionSpace() {
	elem = nullptr;	//元素数量
	curNum = 0;
	max = 0;
}
void SolutionSpace::Initial(int p) {
	elem = new int[p + 1];		//元素数量
	curNum = 0;
	max = p + 1;
}

//析构函数
SolutionSpace::~SolutionSpace() {
	delete[] elem;
	elem = nullptr;
	curNum = 0;
	max = 0;
}
//添加解元素
void SolutionSpace::addElem(int node) {
	//if (curNum == max) cout << "已经存在p+1个元素，仍在添加。当前p:" << p << endl;;	//已经存在p+1个元素时，调用该方法抛出异常
	elem[curNum++] = node;
}
//依据索引删除解元素
void SolutionSpace::deleteElemByIndex(int index) {
	//if (curNum <= max|| index<0 || index>max) cout<<"少于p+1个元素，仍在删除。当前p:"<<p<<endl;	//少于p+1个元素，调用该方法抛出异常
	for (int i = index; i < max - 1; i++)
		elem[i] = elem[i + 1];
	curNum--;
}
//依据元素删除解元素
void SolutionSpace::deleteElemByElem(int node) {
	//if (curNum < max) cout << "少于p+1个元素，仍在删除。当前p:" << p << endl;	//少于p+1个元素，调用该方法抛出异常
	int index = -1;
	for (int i = 0; i < curNum; i++) {
		if (elem[i] == node) {
			index = i;
			break;
		}
	}
	for (int i = index; i < max - 1; i++)
		elem[i] = elem[i + 1];
	curNum--;
}


Node Vi[MAXNODENUM];				//覆盖关系
Node Ci[MAXNODENUM];				//被覆盖关系
SolutionSpace X;							//解空间
int swapi, swapj;
//加载文件，构建数据结构
int loadFile(string filePath) {
	FILE* file = fopen(filePath.c_str(), "r");	//打开文件
	if (file == nullptr) {
		cout << "打开文件失败！" << endl;
		exit(-1);
	}
	//首行，两个参数
	fscanf(file,"%d %d",&N,&p);
	int curNode;
	for (int i = 0; i < N; i++) {
		fscanf(file,"%d",&coverNum[i]);	//读入参数
		for (int j = 0; j < coverNum[i]; j++) {
			fscanf(file, "%d", &curNode);
			//添加覆盖和被覆盖关系
			Vi[i].addCoverNode(curNode);
			Ci[curNode].addCoverNode(i);
		}
	}
	fclose(file);
	return 0;
}

bool beOptimized[MAXNODENUM]{ false };		//当前节点组是否已经被优化

//优化搜索空间
void optimizeSearchSpace() {
	int ret = -2;
	int curNode = -1;
	cout << "集合优化！" << endl;
	//检索覆盖性集合，将相等的集合或者被包含的集合优化成单一覆盖单元，且tt值设置为∞
	for (int i = 0; i < N; i++) {
		for (int j = i+1; j < N; j++) {
			if (beOptimized[i])	break;						//跳出本层循环
			if (beOptimized[j] || i == j) continue;		//相同或者已经被优化的优化项
			//比较两个集合关系
			ret = cmpSet(Vi[i],Vi[j]);
			if (ret == -1) {		//Vi[i] ∈ Vi[j]，对Vi[i]进行优化
				//cout << "集合"<<i<<"集合"<<j<<"存在包含关系，优化集合：" << i << endl;
				beOptimized[i] = true;
				tabuList[i] = INT32_MAX;
				while (Vi[i].curNum > 0) {
					//挨个解除被覆盖关系
					curNode = Vi[i].coverNode[0];
					Ci[curNode].deleteCoverNode(i);		//解除其它集合中被i覆盖的关系
					Vi[i].deleteCoverNode(curNode);		//解除i的覆盖关系
				}
				Vi[i].addCoverNode(i);
				Ci[i].addCoverNode(i);//设置对自己的覆盖
			}
			else if (ret == 1) {//对V[j]进行优化
				//cout << "集合" << i << "集合" << j << "存在包含关系，优化集合：" << j << endl;
				beOptimized[j] = true;
				tabuList[j] = INT32_MAX;
				while (Vi[j].curNum > 0) {
					//挨个解除被覆盖关系，使得仅覆盖自己
					curNode = Vi[j].coverNode[0];
					Ci[curNode].deleteCoverNode(j);		//解除其它集合中被i覆盖的关系
					Vi[j].deleteCoverNode(curNode);		//解除i的覆盖关系
				}
				Vi[j].addCoverNode(j);
				Ci[j].addCoverNode(j);//设置对自己的覆盖
			}
		}
	}
}

//比较集合关系,若不存在包含关系->返回0；若V1∈V2返回-1，若V1包含V2或者V1==V2，则返回1
int cmpSet(Node&V1,Node&V2) {
	if (V1.curNum >= V2.curNum) {
		//V1中元素较多，将V2中元素放到V1中进行包含型判断，若不符合，则返回0，若全都符合，则返回1
		for (int i = 0; i < V2.curNum; i++) {
			if (V1.containElem(V2.coverNode[i]) == false) return 0;
		}
		return 1;
	}
	else {
		//V2中元素较多，将V1中元素放到V2中进行包含性判断，若不符合，返回0，若全都符合，返回-1
		for (int i = 0; i < V1.curNum; i++) {
			if (V2.containElem(V1.coverNode[i]) == false) return 0;
		}
		return -1;
	}
}


// 贪心方法求初始解
void InitialSolution_Greedy() {
	optimizeSearchSpace();
	//初始化权值
	for (int i = 0; i < N; i++) {
		weight[i] = 1;					//权值初始化
		delta[i] = Vi[i].curNum;		//δ 与初始覆盖相同
		nodeBeCoveredNum[i] = 0;
	}
	//解空间初始化
	X.Initial(p);
	lastUx = bestUx = Ux = N;		//初始所有节点未覆盖
	//依次查找最大的δ，依次添加
	int curNode = -1;
	int curMax = -1;
	int curNum = 0;
	for (int i = 0; i < N; i++) {
		if (Ci[i].curNum == 1) {	//唯一性覆盖，将其加入解空间，并将tt设置为无穷大
			addCenter(Ci[i].coverNode[0],0);
			curNum++;
			tabuList[Ci[i].coverNode[0]] = INT32_MAX;
		}
	}
	for (int j = curNum; j < p; j++) {		//添加p个节点
		curMax = -1;
		for (int i = 0; i < N; i++) {
			if (isAlive[i]) continue;
			if (delta[i] > curMax&&tabuList[i]==0) {
				curMax = delta[i];
				curNode = i;
			}
			else if (delta[i] == curMax && tabuList[i] == 0 && rand() % 2 == 0) {
				curNode = i;
			}
		}//找到最大的
		addCenter(curNode,0);	//添加节点
	}
}

//添加中心节点
bool addCenter(int node,int iter) {
	isAlive[node] = true;
	X.addElem(node);	//将node加入到解空间中
	/*//检查解空间与当前alive控制的解空间的一致性
	if (Ux > 1000) {
		int get = false;
		for (int i = 0; i < N; i++) {
			if (isAlive[i]) {
				get = false;
				//在解空间中查找该项，如果没有，打印error
				for (int j = 0; j < X.curNum; j++) {
					if (X.elem[j] == i) {
						get = true;
						break;
					}
				}
				if (get == false) { cout << "error!解空间中缺少相关项" << endl;	cout << "上一次swapi与swapj分别为：" << swapi << " " << swapj << endl; }
			}
			else {
				//在解空间中查找该项，如果有，打印error
				for (int j = 0; j < X.curNum; j++) {
					if (X.elem[j] == i) {
						cout << "error!解空间中项数过多" << endl;
						cout << "上一次swapi与swapj分别为：" << swapi << " " << swapj << endl;
					}
				}
			}
		}
	}
	
	//end*/
	//横向和纵向更新
	int curNode = -1;
	addCenterIter[node] = iter;		//记录时间戳
	//对于当前i覆盖的所有节点（横向）
	for (int i = 0; i < Vi[node].curNum; i++) {
		curNode = Vi[node].coverNode[i];
		//覆盖项增加
		nodeBeCoveredNum[curNode] ++;
		if (nodeBeCoveredNum[curNode]==1) {			//从零到1，除node之外的全部能够覆盖curNode的节点的δ值更新
			//设置唯一性覆盖
			beCoverdeOnly[curNode] = node;
			Ux--;		//未覆盖节点数量-1
			//纵向更新：除node之外的全部能够覆盖curNode的节点的δ值更新
			for (int j = 0; j < Ci[curNode].curNum; j++) {
				if (Ci[curNode].coverNode[j] != node) {
					delta[Ci[curNode].coverNode[j]] -= weight[curNode];
				}
			}
		}
		else if (nodeBeCoveredNum[curNode]==2) {	//从1到2，仅更新已激活覆盖的节点，即前置的唯一性覆盖
			delta[beCoverdeOnly[curNode]] -= weight[curNode];
		}
	}
	return true;
}

//删除中心节点
bool deleteCenter(int node) {
	isAlive[node] = false;
	X.deleteElemByElem(node);	//将node从解空间移除
	//检查解空间与当前alive控制的解空间的一致性
	/*int get = false;
	for (int i = 0; i < N; i++) {
		if (isAlive[i]) {
			get = false;
			//在解空间中查找该项，如果没有，打印error
			for (int j = 0; j < X.curNum; j++) {
				if (X.elem[j] == i) {
					get = true;
					break;
				}
			}
			if (get == false) cout << "error!" << endl;
		}
		else {
			//在解空间中查找该项，如果有，打印error
			for (int j = 0; j < X.curNum; j++) {
				if (X.elem[j] == i) {
					cout << "error!" << endl;
				}
			}
		}
	}*/
	//end
	int curNode = -1;
	//对于node覆盖的每一个节点curNode
	for (int i = 0; i < Vi[node].curNum; i++) {
		curNode = Vi[node].coverNode[i];
		nodeBeCoveredNum[curNode] --;		//覆盖中心数量-1
		if (nodeBeCoveredNum[curNode] == 0) {		//从1到0，除node之外的全部能覆盖curNode的节点的δ值更新
			Ux++;				//未覆盖节点数量+1
			for (int j = 0; j < Ci[curNode].curNum; j++) {
				if (Ci[curNode].coverNode[j] != node) {
					delta[Ci[curNode].coverNode[j]] += weight[curNode];
				}
			}
		}
		else if (nodeBeCoveredNum[curNode] == 1) {	//从2到1，更新唯一覆盖的δ，并更新唯一性覆盖
			for(int j = 0;j < Ci[curNode].curNum;j++)
				if (isAlive[Ci[curNode].coverNode[j]] == true) {
					beCoverdeOnly[curNode] = Ci[curNode].coverNode[j];
					delta[Ci[curNode].coverNode[j]] += weight[curNode];
					break;
				}
		}
	}
}//check

void MakeMove(int swapi, int swapj,int iter) {
	if (swapi == -1||swapj==-1) return;
	addCenter(swapi,iter);
	deleteCenter(swapj);
}


typedef struct deltaBackup {	//delta值回溯
	int index[MAXNODENUM];
	int value[MAXNODENUM];
	int num = 0;
}deltaBackup;
void addDeltaBackupNode(deltaBackup& deltabackup, int index, int value) {
	for (int i = 0; i < deltabackup.num; i++)
		if (index == deltabackup.index[i]) return;		//存在相同值，不再添加
	deltabackup.index[deltabackup.num] = index;
	deltabackup.value[deltabackup.num] = value;
	deltabackup.num++;
}
void backupDelt(deltaBackup &deltabackup) {
	for (int i = 0; i < deltabackup.num; i++)
		delta[deltabackup.index[i]] = deltabackup.value[i];
	deltabackup.num = 0;		//完成回溯后，重置num值
}
deltaBackup deltabackup;
//尝试添加节点
void TryToOpenCenter(int node) {
	//对于每一个node覆盖的节点
	int curNode = -1;
	for (int i = 0; i < Vi[node].curNum; i++) {
		curNode = Vi[node].coverNode[i];
		//如果该节点被唯一性覆盖，则更改其δ
		if (nodeBeCoveredNum[curNode]==1) {
			addDeltaBackupNode(deltabackup,beCoverdeOnly[curNode], delta[beCoverdeOnly[curNode]]);
			delta[beCoverdeOnly[curNode]] -= weight[curNode];
		}
	}
}


//查找交换对
void FindPair(int iter) {
	int obj = 9999;
	swapi = -1;
	swapj = -1;
	//寻找一个未被覆盖的节点
	int v = rand() % Ux;
	for (int i = 0; i < N; i++) {
		if (nodeBeCoveredNum[i]==0) {
			if (v == 0) {
				v = i;
				break;
			}
			else v--;
		}
	}
	//对于每一个能覆盖节点v的节点i
	int curi = -1, curj = -1;
	for (int i = 0; i < Ci[v].curNum; i++) {
		curi = Ci[v].coverNode[i];	//能覆盖v的节点curi
		TryToOpenCenter(curi);	//尝试添加curi
		//对于X中所有节点
		for (/*int curj = 0; curj < N; curj++*/int j = 0; j < X.curNum;j++) {//解空间中寻找
			curj = X.elem[j];
			if (/*isAlive[curj] &&*/ iter > tabuList[curi] && iter > tabuList[curj]) {
				if (delta[curj] - delta[curi] < obj) {
					obj = delta[curj] - delta[curi];
					swapi = curi;
					swapj = curj;
				}
				else if (delta[curj] - delta[curi] == obj) {
					if (rand() % 2 == 0) {	//随机修改
						swapi = curi; swapj = curj;
					}
				}
			}
		}
		backupDelt(deltabackup);
	}
}
#define MAXP 500
int _bestNodeBeCoveredNum[MAXNODENUM]{ 0 };
int _bestBeCoveredOnly[MAXNODENUM]{ 0 };
bool _bestIsAlive[MAXNODENUM]{ 0 };
int _bestDelta[MAXNODENUM]{ 0 };
int _bestWeight[MAXNODENUM]{ 0 };
int _bestUx = 0;
int _bestX[MAXP]{ 0 };
int _bestIter = 0;
int _bestTabuList[MAXNODENUM]{ 0 };
int curRecursion;		//记录在哪一层递归中获得的最值，优先储存最底层递归
bool _recursiontabu[MAXNODENUM]{0};
void neighborhoodCheck(int cur, int iter);
bool isChange = false;
//扩大领域搜索范围，跳出局部最优，参数num是考虑的邻域替换组数
int FindPairJumpOutLocal(int num,int iter) {
	//储存delta，isAlive等值后，事实上执行多次makeMove，选中最小，如果没有优化项，则回退到上一个最优解的即可
	//需要储存的结构
	//cout << "执行多步邻域全搜索，递归层级" <<num<< endl;
	isChange = false;
	//从最优结构开始执行多步邻域全搜索
	for (int m = 0; m < N; m++) {
		nodeBeCoveredNum[m] = _bestNodeBeCoveredNum[m];
		beCoverdeOnly[m] = _bestBeCoveredOnly[m];
		isAlive[m] = _bestIsAlive[m];
		delta[m] = _bestDelta[m];
		weight[m] = _bestWeight[m];
		tabuList[m] = _bestTabuList[m];
		if (tabuList[m] < INT32_MAX)
			_recursiontabu[m] = false;	//未禁
		else _recursiontabu[m] = true;	//禁忌
	}
	Ux = _bestUx;
	for (int m = 0; m < p; m++)
		X.elem[m] = _bestX[m];
	curRecursion = num;
	int cur = num;
	
	neighborhoodCheck(cur, iter);
	//读取最优解结构
	for (int m = 0; m < N; m++) {
		nodeBeCoveredNum[m] = _bestNodeBeCoveredNum[m];
		beCoverdeOnly[m] = _bestBeCoveredOnly[m];
		isAlive[m] = _bestIsAlive[m];
		delta[m] = _bestDelta[m];
	}
	iter = _bestIter;
	Ux = _bestUx;
	for (int m = 0; m < p; m++)
		X.elem[m] = _bestX[m];
	//cout << "多步邻域全搜索执行完毕，最优解"<<_bestUx << endl;
	//if (isChange) cout << "已替换" << endl;
}


//递归的邻域检索，使用cur作为跳出标志
void neighborhoodCheck(int cur,int iter) {
	if (cur == 0) return;
	//cout << "第" << cur << "层递归" << endl;
	//储存所有相关结构
	int* curNodeBeCoveredNum = new int[N] {0};
	int* curBeCoveredOnly = new int[N] {0};
	bool* curIsAlive = new bool[N] {0};
	int* curDelta = new int[N] {0};
	int curUx;
	int* curX = new int[p] {0};
	for (int i = 0; i < N; i++) {
		curNodeBeCoveredNum[i] = nodeBeCoveredNum[i];
		curBeCoveredOnly[i] = beCoverdeOnly[i];
		curIsAlive[i] = isAlive[i];
		curDelta[i] = delta[i];
	}
	curUx = Ux;
	for (int j = 0; j < p; j++)
		curX[j] = X.elem[j];
	//对于所有v
	int u = -1;
	for (int i = 0; i < N; i++) {
		if (nodeBeCoveredNum[i] == 0) {		//当前节点未被覆盖
			//对于每一个覆盖i的节点u
			for (int j = 0; j < Ci[i].curNum; j++) {
				u = Ci[i].coverNode[j];
				if (_recursiontabu[u]==false) {	//未禁
					//对于所有j∈X
					for (int k = 0; k < X.curNum; k++) {
						if (_recursiontabu[X.elem[k]]) continue;	//禁忌
						//执行makeMove
						MakeMove(u,X.elem[k],iter);
						//执行后设置禁忌表，尝试过添加的，或者尝试过删除的，不再考虑
						_recursiontabu[u] = true;
						_recursiontabu[X.elem[k]] = true;
						//检测最优并赋值
						if (Ux < _bestUx || (Ux == _bestUx && cur < curRecursion) || (Ux == _bestUx && curRecursion == cur && rand() % 2 == 0)) {
							//复制最值
							//cout << "替换" << endl;
							isChange = true;
							for (int m = 0; m < N; m++) {
								_bestNodeBeCoveredNum[m] = nodeBeCoveredNum[m];
								_bestBeCoveredOnly[m] = beCoverdeOnly[m];
								_bestIsAlive[m] = isAlive[m];
								_bestDelta[m] = delta[m];
							}
							_bestUx = Ux;
							for (int m = 0; m < p; m++)
								_bestX[m] = X.elem[m];
							curRecursion = cur;
						}
						if (Ux == 0) return;//已获得最优值，复制数据，返回
						//递归执行写一部邻域试探
						neighborhoodCheck(cur-1,iter);
						//恢复上一轮的结构
						for (int m = 0; m < N; m++) {
							nodeBeCoveredNum[m] = curNodeBeCoveredNum[m];
							beCoverdeOnly[m] = curBeCoveredOnly[m];
							isAlive[m] = curIsAlive[m];
							delta[m] = curDelta[m];
						}
						Ux = curUx;
						for (int m = 0; m < p; m++)
							X.elem[m] = curX[m];
					}
				}
			}
		}
	}
	//销毁创建的结构
	delete[] curNodeBeCoveredNum;
	delete[] curBeCoveredOnly;
	delete[] curIsAlive;
	delete[] curDelta;
	delete[] curX;
}

//计算Ux
int testCalculateUx() {
	bool isCovered[MAXNODENUM]{ false };
	//逐个检测
	for (int i = 0; i < N; i++) {
		if (isAlive[i]) {
			for (int j = 0; j < Vi[i].curNum; j++) {
				isCovered[Vi[i].coverNode[j]] = true;
			}
		}
	}
	int res = 0;
	for (int i = 0; i < N; i++)
	{
		if (isCovered[i] == false) {
			res++;
		}
	}
	return res;
}
int begintime;
int timelimit;
void VWTS() {
	InitialSolution_Greedy();
	iter = 1;
	int curIter = 1;					//当前iter，用于检测局部最优
	int jumpIter = 100000;		//增量式局部最优判定
	int curRecursion = 2;			//二级全邻域搜索
	cout << "初始化结束！" << endl;
	int time = clock();
	while (Ux > 0&&clock()-begintime < timelimit) {
		if (curIter > jumpIter) {
			curIter = 1;
			jumpIter += 100000;
			FindPairJumpOutLocal(curRecursion, iter);
			curRecursion++;		//执行一次，邻域检测更进一步
		}
		else {
			FindPair(iter);
			MakeMove(swapi, swapj, iter);
		}
		if (Ux < bestUx) {
			bestUx = Ux;
			curIter = 1;	//重置
			curRecursion = 2;
			for (int m = 0; m < N; m++) {
				_bestNodeBeCoveredNum[m] = nodeBeCoveredNum[m];
				_bestBeCoveredOnly[m] = beCoverdeOnly[m];
				_bestIsAlive[m] = isAlive[m];
				_bestDelta[m] = delta[m];
				_bestWeight[m] = weight[m];
				_bestTabuList[m] = tabuList[m];
			}
			_bestUx = Ux;
			_bestIter = iter;
			for (int m = 0; m < p; m++)
				_bestX[m] = X.elem[m];
		}
		else if (Ux >= lastUx) {
			//未被覆盖的节点，权重+1
			for (int i = 0; i < N; i++) {
				if (nodeBeCoveredNum[i]==0) {
					weight[i]++;
					//更新位覆盖节点的权值
					for (int j = 0; j < Ci[i].curNum; j++) {
						delta[Ci[i].coverNode[j]] += 1;
					}
				}
			}
		}
		tabuList[swapi] = iter + 1;
		tabuList[swapj] = iter + 1;
		lastUx = Ux;
		iter++;
		curIter++;
		//cout << "迭代次数：iter=" << iter - 1 << endl;
		//if (iter % 100000 == 0) cout << "迭代次数："<< iter <<"cur:" << Ux << "\tbest:" << bestUx << endl;
	}
	cout << "迭代次数：" << iter - 1 << endl;
	cout << "迭代过程耗时：" << clock() - time << "ms" << endl;
}



/*int main() {
	std::cout << "filePath:" << endl;
	string filePath;
	cin >> filePath;
	int random = time(NULL)%65536;
	//int random = 0;
	cout << "随机数种子" << random << endl;
	srand(random);
	loadFile(ADDR + filePath);
	int time = clock();
	VWTS();
	cout << "总耗时：" << clock() - time << "ms" << endl;
	std::cout << "Center Vertex:" << endl;
	for (int i = 0; i < N; i++) {
		if (isAlive[i]) std::cout << i << endl;
	}
	std::cout << "未覆盖节点数：" << testCalculateUx() << endl;
	return 0;
}*/

int main(int argv, char** args) {
	//四个参数：算例文件路径, 输出解文件路径, 运行时间上限 (单位为秒) 和随机种子 (0-65535).
	if (argv < 4) {
		cout << "参数数量不足，退出。" << endl;
		return 0;
	}
	int random;
	if (argv == 4) {//缺少最后一个参数，随机生成随机数字
		random = time(NULL) % 65536;
	}
	else {
		random = atoi(args[4]);
	}
	timelimit = atoi(args[3]) * 1000;		//时间上限
	cout << "随机数种子:" << random << endl;
	cout << "时间上限:" << timelimit << endl;
	srand(random);
	loadFile(args[1]);
	begintime = clock();
	VWTS();
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

