#include "TensureSearch.h"
//构造函数
TensureSearch::TensureSearch(int N, int k, int randomSeed) :N(N), k(k) {
	if (N <= 0 || k <= 0) throw"Error!";
	sol = new int[N] {0};
	best_Sol = new int[N] {0};
	tabuTensure = new int[N * k]{ 0 };
	adjiacent_Color_Table = new int[N * k]{ 0 };
	this->randomSeed = randomSeed;
	srand(randomSeed);	//设置随机数种子
}
//析构函数
TensureSearch::~TensureSearch() {
	delete[] sol;
	delete[] best_Sol;
	delete[] tabuTensure;
	delete[] adjiacent_Color_Table;
}

//1. 评价函数计算
int TensureSearch::CalculateF(Graph &graph) {
	//遍历每一条边，判断冲突数
	//由于无向图邻接矩阵为对称矩阵，仅计算上半三角阵即可
	int res = 0;
	for (int i = 0; i < N; i++) {
		for (int j = i + 1; j < N; j++) {
			if (graph[i][j] == 1) {
				if (sol[i] == sol[j]) res++;
			}
		}
	}
	return res;
}
//检测冲突数
int TensureSearch::TestConflict(Graph& graph) {
	int res = 0;
	for (int i = 0; i < N; i++) {
		for (int j = i + 1; j < N; j++) {
			if (graph[i][j]==1&&best_Sol[i] == best_Sol[j]) res++;
		}
	}
	return res;
}

//2.TabuSearch
void TensureSearch::TabuSearch(Graph &graph, int maxIter) {
	Initialization(graph);
	int iter = 0;
	int u = 0, vi = 0, vj = 0, delt = 0;
	int begintime = clock();
	while ((iter++) < maxIter&&f > 0) {
		FindMove(u, vi, vj, delt, iter);
		MakeMove(u, vi, vj, delt, iter, graph);
	}
	cout << "耗时：" << clock() - begintime << endl;
	//解情况
	if (f == 0)		cout << "已求解" << endl;
	else cout << "未求得解" << endl;
	cout << "随机数种子:" << randomSeed << endl;
	cout << "迭代次数：" << iter - 1 << "\t" << "冲突数：" << best_f << endl;

}

//HEA_Tabu Search
void TensureSearch::TabuSearch_HEA(Graph& graph, int maxIter,int *solution,int &cur_f) {
	//跳过随即赋值，将迭代后的结果储存到sol中
	int iter = 0;
	int u = 0, vi = 0, vj = 0, delt = 0;
	Initialization(graph,solution);
	while ((iter++) < maxIter && f > 0) {
		FindMove(u, vi, vj, delt, iter);
		MakeMove(u, vi, vj, delt, iter, graph);
	}
	//将最优结果储存到sol中
	for (int i = 0; i < N; i++) {
		solution[i] = best_Sol[i];
		cur_f = best_f;
	}
}



//3.初始化函数.bool值为true时跳过sol和best_sol随机赋值过程
void TensureSearch::Initialization(Graph &graph) {
	for (int i = 0; i < N; i++) {
		sol[i] = best_Sol[i] = rand() % k;	//分配随机颜色
	}
	f = best_f = CalculateF(graph);
	//计算Adjacent_Color_Table(着某种颜色时邻冲突节点个数)
	int cur_adj = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < k; j++) {
			//对于每个节点的每一种颜色
			cur_adj = 0;
			//判断节点graph[i]着颜色j时，冲突节点个数
			for (int m = 0; m < N; m++) {
				//对于每一个邻接点
				if (graph[i][m] != 1 || m == i) continue;	//非邻接点或者相同节点跳过
				if (sol[m] == j) cur_adj++;	//颜色相同时冲突
			}
			adjiacent_Color_Table[i * k + j] = cur_adj;	//节点i着色j时冲突数
		}
	}
}

void TensureSearch::Initialization(Graph& graph, int* solution) {
	for (int i = 0; i < N; i++) {
		best_Sol[i] = sol[i] = solution[i];
	}
	f = best_f = CalculateF(graph);
	//计算Adjacent_Color_Table(着某种颜色时邻冲突节点个数)
	int cur_adj = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < k; j++) {
			//对于每个节点的每一种颜色
			cur_adj = 0;
			//判断节点graph[i]着颜色j时，冲突节点个数
			for (int m = 0; m < N; m++) {
				//对于每一个邻接点
				if (graph[i][m] != 1 || m == i) continue;	//非邻接点或者相同节点跳过
				if (sol[m] == j) cur_adj++;	//颜色相同时冲突
			}
			adjiacent_Color_Table[i * k + j] = cur_adj;	//节点i着色j时冲突数
		}
	}
}


//4.FindMove()
bool TensureSearch::FindMove(int& u, int& vi, int& vj, int& delt, int iter) {
	int cur_delt = 0;
	int best_u = 0, best_vi = 0, best_vj = 0, best_delt = 999999;
	int non_best_u = 0, non_best_vi = 0, non_best_vj = 0, non_best_delt = 999999;
	for (int i = 0; i < N; i++) {
		if (adjiacent_Color_Table[i * k + sol[i]] > 0) {
			for (int j = 0; j < k; j++) {
				if (sol[i] == j)  continue;	//跳过同色移动
				cur_delt = adjiacent_Color_Table[i * k + j] - adjiacent_Color_Table[i * k + sol[i]];
				if (tabuTensure[i * k + j] < iter) {	//更新bestmove
					if (cur_delt < best_delt||(cur_delt == best_delt&&rand()%2==0)) {	//值相同时，随机决定是否更新（体现随机选择性）
						best_u = i;
						best_vi = sol[i];
						best_vj = j;
						best_delt = cur_delt;
					}
				}
				else {	//更新nonbestmove
					if (cur_delt < non_best_delt || (cur_delt == non_best_delt && rand() % 2 == 0)) {
						non_best_u = i;
						non_best_vi = sol[i];
						non_best_vj = j;
						non_best_delt = cur_delt;
					}
				}
			}
		}
	}
	if (f + non_best_delt < best_f && non_best_delt < best_delt) {
		u = non_best_u;
		vi = non_best_vi;
		vj = non_best_vj;
		delt = non_best_delt;
	}
	else {
		u = best_u;
		vi = best_vi;
		vj = best_vj;
		delt = best_delt;
	}
	if (vi == 0 && vj == 0) exit(-1);
	return true;
}
//5.MakeMove()
void TensureSearch::MakeMove(int u, int vi, int vj, int delt, int iter, Graph &graph) {
	sol[u] = vj;	//更新颜色
	//f = CalculateF(graph);
	f = f + delt;	//更新冲突值
	
	//更新历史最好冲突
	if (f < best_f) {
		best_f = f;
		for (int i = 0; i < N; i++)	best_Sol[i] = sol[i];
	}
	//cout << "第" << iter << "轮迭代,冲突值：" << f << endl;
	//cout << "最佳冲突值：" << best_f << endl;
	tabuTensure[u * k + vi] = iter + f + rand() % 10;
	//update the adj_Color_Table
	//与u相邻的节点的颜色为vi的冲突数-1，颜色为vj的冲突数+1
	for (int i = 0; i < N; i++) {
		if (u == i) continue;
		if (graph[u][i] == 1) {
			adjiacent_Color_Table[i * k + vi] -= 1;
			adjiacent_Color_Table[i * k + vj] += 1;
		}
	}
}

void TensureSearch::printColor() {
	for (int i = 0; i < N; i++) {
		cout << i << " " << best_Sol[i] << endl;
	}
	return;
}

