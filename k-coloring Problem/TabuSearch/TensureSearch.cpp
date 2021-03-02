#include "TensureSearch.h"
//���캯��
TensureSearch::TensureSearch(int N, int k, int randomSeed) :N(N), k(k) {
	if (N <= 0 || k <= 0) throw"Error!";
	sol = new int[N] {0};
	best_Sol = new int[N] {0};
	tabuTensure = new int[N * k]{ 0 };
	adjiacent_Color_Table = new int[N * k]{ 0 };
	this->randomSeed = randomSeed;
	srand(randomSeed);	//�������������
}
//��������
TensureSearch::~TensureSearch() {
	delete[] sol;
	delete[] best_Sol;
	delete[] tabuTensure;
	delete[] adjiacent_Color_Table;
}

//1. ���ۺ�������
int TensureSearch::CalculateF(Graph &graph) {
	//����ÿһ���ߣ��жϳ�ͻ��
	//��������ͼ�ڽӾ���Ϊ�Գƾ��󣬽������ϰ������󼴿�
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
//����ͻ��
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
	cout << "��ʱ��" << clock() - begintime << endl;
	//�����
	if (f == 0)		cout << "�����" << endl;
	else cout << "δ��ý�" << endl;
	cout << "���������:" << randomSeed << endl;
	cout << "����������" << iter - 1 << "\t" << "��ͻ����" << best_f << endl;

}

//HEA_Tabu Search
void TensureSearch::TabuSearch_HEA(Graph& graph, int maxIter,int *solution,int &cur_f) {
	//�����漴��ֵ����������Ľ�����浽sol��
	int iter = 0;
	int u = 0, vi = 0, vj = 0, delt = 0;
	Initialization(graph,solution);
	while ((iter++) < maxIter && f > 0) {
		FindMove(u, vi, vj, delt, iter);
		MakeMove(u, vi, vj, delt, iter, graph);
	}
	//�����Ž�����浽sol��
	for (int i = 0; i < N; i++) {
		solution[i] = best_Sol[i];
		cur_f = best_f;
	}
}



//3.��ʼ������.boolֵΪtrueʱ����sol��best_sol�����ֵ����
void TensureSearch::Initialization(Graph &graph) {
	for (int i = 0; i < N; i++) {
		sol[i] = best_Sol[i] = rand() % k;	//���������ɫ
	}
	f = best_f = CalculateF(graph);
	//����Adjacent_Color_Table(��ĳ����ɫʱ�ڳ�ͻ�ڵ����)
	int cur_adj = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < k; j++) {
			//����ÿ���ڵ��ÿһ����ɫ
			cur_adj = 0;
			//�жϽڵ�graph[i]����ɫjʱ����ͻ�ڵ����
			for (int m = 0; m < N; m++) {
				//����ÿһ���ڽӵ�
				if (graph[i][m] != 1 || m == i) continue;	//���ڽӵ������ͬ�ڵ�����
				if (sol[m] == j) cur_adj++;	//��ɫ��ͬʱ��ͻ
			}
			adjiacent_Color_Table[i * k + j] = cur_adj;	//�ڵ�i��ɫjʱ��ͻ��
		}
	}
}

void TensureSearch::Initialization(Graph& graph, int* solution) {
	for (int i = 0; i < N; i++) {
		best_Sol[i] = sol[i] = solution[i];
	}
	f = best_f = CalculateF(graph);
	//����Adjacent_Color_Table(��ĳ����ɫʱ�ڳ�ͻ�ڵ����)
	int cur_adj = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < k; j++) {
			//����ÿ���ڵ��ÿһ����ɫ
			cur_adj = 0;
			//�жϽڵ�graph[i]����ɫjʱ����ͻ�ڵ����
			for (int m = 0; m < N; m++) {
				//����ÿһ���ڽӵ�
				if (graph[i][m] != 1 || m == i) continue;	//���ڽӵ������ͬ�ڵ�����
				if (sol[m] == j) cur_adj++;	//��ɫ��ͬʱ��ͻ
			}
			adjiacent_Color_Table[i * k + j] = cur_adj;	//�ڵ�i��ɫjʱ��ͻ��
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
				if (sol[i] == j)  continue;	//����ͬɫ�ƶ�
				cur_delt = adjiacent_Color_Table[i * k + j] - adjiacent_Color_Table[i * k + sol[i]];
				if (tabuTensure[i * k + j] < iter) {	//����bestmove
					if (cur_delt < best_delt||(cur_delt == best_delt&&rand()%2==0)) {	//ֵ��ͬʱ����������Ƿ���£��������ѡ���ԣ�
						best_u = i;
						best_vi = sol[i];
						best_vj = j;
						best_delt = cur_delt;
					}
				}
				else {	//����nonbestmove
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
	sol[u] = vj;	//������ɫ
	//f = CalculateF(graph);
	f = f + delt;	//���³�ͻֵ
	
	//������ʷ��ó�ͻ
	if (f < best_f) {
		best_f = f;
		for (int i = 0; i < N; i++)	best_Sol[i] = sol[i];
	}
	//cout << "��" << iter << "�ֵ���,��ͻֵ��" << f << endl;
	//cout << "��ѳ�ͻֵ��" << best_f << endl;
	tabuTensure[u * k + vi] = iter + f + rand() % 10;
	//update the adj_Color_Table
	//��u���ڵĽڵ����ɫΪvi�ĳ�ͻ��-1����ɫΪvj�ĳ�ͻ��+1
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

