#include "ACO.h"

int tmp = 0;

double alfa = 1, beta = 7, sigm = 3, ro = 0.85, th = 80;

double distances[100][100];
double pheromones[100][100];

auto startTime = chrono::system_clock::now();
auto bestTime = chrono::system_clock::now();

double passingTime;
int iteration, bestIteration;

float calculateCost(Solution& solution) {
	double cost = 0;
	for (int i = 0; i < solution.routes.size(); i++) {
		vector<Point>& p = solution.routes[i].points;
		int a = 0, b = 0;
		for (int j = 0; j < p.size(); j++) {
			b = p[j].index;
			cost += distances[min(a, b)][max(a, b)];
			//cout << "i1: " << a << " i2: " << b << " cost: " << distances[min(a, b)][max(a, b)] << " totalCost: " << cost << endl;
			a = b;
		}
		b = 0;
		cost += distances[min(a, b)][max(a, b)];
		//cout << "i1: " << a << " i2: " << b << " cost: " << distances[min(a, b)][max(a, b)] << " totalCost: " << cost << endl;
	}
	return cost;
}

Solution buildSolutionPerAnt(Dataset& dataset) {
	Solution solution;
	//产生备选点集，排除0（起点）
	vector<int> indexs;
	for (int i = 1; i < dataset.pointNum; i++) indexs.push_back(i);
	while (!indexs.empty()) {
		Route route;
		//for (int l = 0; l < 30; l++) cout << rand() % 31 << endl;
		//system("pause");
		//这里的sign是indexs的下标，index才是实际点的标号（因为中间的点可能被删去，所以每次随机的只能是数组下标）
		int sign = rand() % indexs.size();
		int index = indexs[sign];
		int curCapacity = dataset.capacity - dataset.points[index].demand;
		route.points.push_back(dataset.points[index]);
		//cout << "index: " << index << " right: " << indexs[sign] << " shanchu: " << *(indexs.begin() + sign) << " diyige: " << *(indexs.begin() + 0) << endl;
		indexs.erase(indexs.begin() + sign);
		//cout << "indexs:" << endl; for (int b = 0; b < indexs.size(); b++) cout << indexs[b] << endl;
		
		//system("pause");
		while (!indexs.empty()) {
			//算法核心：按概率随机选择下一个点
			vector<double> probabilities;
			//注意：i也只是indexs的下标，要映射成实际点的标号
			//debug
			int debugFlag = 0;
			for (int i = 0; i < indexs.size(); i++) {
				int i1 = min(indexs[i], index);
				int i2 = max(indexs[i], index);
				//cout <<"index: " <<index<<  " i1: " << i1 << " i2: " << i2 << endl;
				double probability = pow(pheromones[i1][i2], alfa) * pow(1 / distances[i1][i2], beta);
				if (probability == 0) debugFlag = indexs[i];
				probabilities.push_back(probability);
			}
			//debug
			if (debugFlag) {
				int i1 = min(debugFlag, index);
				int i2 = max(debugFlag, index);
				cout << "tmp: " << tmp << endl; for (int a = 0; a < probabilities.size(); a++) cout << a << " index: " << indexs[a] << " probability: " << probabilities[a] << endl;
				cout << endl << "curIndex: " << index << endl << " debugFlag: " << debugFlag << " phe: " << pheromones[i1][i2] << " dis: " << distances[i1][i2] << endl;
				cout << "a: " << pow(pheromones[i1][i2], alfa) << " b: " << pow(1 / distances[i1][i2], beta) << " a*b: " << pow(pheromones[i1][i2], alfa) * pow(1 / distances[i1][i2], beta) <<endl;
				system("pause");
			}
			//cout << "cur index: " << index << endl;
			//if (tmp > 307) { cout << "tmp: " << tmp << endl; for (int a = 0; a < probabilities.size(); a++) cout << a << " probability: " << probabilities[a] << endl; }
			//system("pause");
			default_random_engine generator;
			discrete_distribution<int> dist{probabilities.begin(), probabilities.end()};
			//再次注意：这里按概率随机生成的是indexs数组的下标，还要映射成点的标号
			sign = dist(generator);
			index = indexs[sign];
			//for (int c = 0; c < 20; c++) { index = dist(generator); cout << "xiabiao: " << index << " index: " << indexs[index] << endl; }
			//sign = dist(generator);
			//index = indexs[sign];
			//cout << "chosen index: " << index << endl;
			//system("pause");

			curCapacity -= dataset.points[index].demand;
			if (curCapacity > 0) {
				//注意区分sign和index
				route.points.push_back(dataset.points[index]);
				indexs.erase(indexs.begin() + sign);
			}
			else break;
		}
		solution.routes.push_back(route);
	}
	//计算当前解的cost
	solution.cost = calculateCost(solution);
	//cout << solution.cost << endl;
	return solution;
}

void updatePheromones(vector<Solution>& solutions, Dataset& dataset, Solution& bestSolution, int& bestFlag) {
	//求平均cost
	float sum = 0;
	for (int i = 0; i < solutions.size(); i++) sum += solutions[i].cost;
	float avgCost = sum / solutions.size();
	//更新信息素(蒸发)
	for (int i = 0; i < dataset.points.size(); i++) {
		for (int j = i; j < dataset.points.size(); j++) {
			//注意：这里的+可能算术溢出，要强转为宽类型
			pheromones[i][j] = (double(ro) + double(th / avgCost)) * pheromones[i][j];
		}
	}
	//按cost排序
	sort(solutions.begin(), solutions.end());
	if (bestFlag != 0) {
		if (solutions[0].cost < bestSolution.cost) {
			bestSolution = solutions[0];
			//bestTime = time(nullptr);
			bestTime = chrono::system_clock::now();
			//passingTime = bestTime	- startTime;
			bestIteration = iteration;
		}
		for (int i = 0; i < bestSolution.routes.size(); i++) {
			vector<Point>& p = bestSolution.routes[i].points;
			//给最优解的所有路径增加信息素
			//注意：这里要不要加上起点和终点的信息素呢？
			for (int j = 0; j < p.size() - 1; j++) {
				int i1 = p[j].index;
				int i2 = p[j + 1].index;
				pheromones[min(i1, i2)][max(i1, i2)] += sigm / bestSolution.cost;
			}
		}
	}
	else {
		bestSolution = solutions[0];
		bestFlag = 1;
		bestTime = chrono::system_clock::now();
		//bestTime = time(nullptr) - startTime;
		bestIteration = iteration;
	}
	//给精英蚂蚁（前sigm）的解上所有路径增加信息素
	for (int i = 0; i < sigm; i++) {
		for (int j = 0; j < solutions[i].routes.size(); j++) {
			vector<Point>& p = solutions[i].routes[j].points;
			for (int k = 0; k < p.size() - 1; k++) {
				int i1 = p[k].index;
				int i2 = p[k + 1].index;
				pheromones[min(i1, i2)][max(i1, i2)] += sigm - (i + 1) / pow(solutions[i].cost, i + 1);
			}
		}
	}
	return;
}

Solution ACO::run(Dataset& dataset, int** map, int timeLimit, int antNum, double inputRo, int inputSigma) {
	ro = inputRo;
	sigm = inputSigma;
	//重要：设置随机数种子
	srand(time(0));
	Solution bestSolution;
	int bestFlag = 0;
	auto startTimeS = time(nullptr);
	startTime = chrono::system_clock::now();
	//生成信息素、边的距离
	for (int i = 0; i < dataset.pointNum; i++)
		for (int j = i; j < dataset.pointNum; j++) {
			Point& p1 = dataset.points[i];
			Point& p2 = dataset.points[j];
			//dis[i][j] = calculateDis(p1, p2);
			vector<Point> v;
			//cout << " x1: " << p1.tx << " y1: " << p1.ty << " x2: " << p2.tx << " y2: " << p2.ty << endl;
			distances[i][j] = 5 * searchRoute(p1, p2, map, v);
			Solution::inter[p1.index][p2.index] = v;
			//distances[i][j] = calculateDis(p1, p2, "EUC");
			//if (distances[i][j] != calculateDis(p1, p2)) { cout << "debug!!" << endl; system("pause"); }
			pheromones[i][j] = 1;
			//cout << " i: " << i << " j: " << j << " dis: " << distances[i][j] << " phe: " << pheromones[i][j] << endl;
		}
	/*
		for (int i = 0; i < dataset.pointNum; i++)
		for (int j = i + 1; j < dataset.pointNum; j++) {
			Point& p1 = dataset.points[i];
			Point& p2 = dataset.points[j];
			//dis[i][j] = calculateDis(p1, p2);
			vector<Point> v;
			//cout << " x1: " << p1.tx << " y1: " << p1.ty << " x2: " << p2.tx << " y2: " << p2.ty << endl;
			distances[i][j] = 5 * searchRoute(p1, p2, map, v);
			initSolution::inter[p1.index][p2.index] = v;
		}
		*/
	//system("pause");
	int iterations = 0;
	while (time(nullptr) - startTimeS <= timeLimit) {
		vector<Solution> solutions;
		for (int j = 0; j < antNum; j++) {
			auto solution = buildSolutionPerAnt(dataset);
			solutions.push_back(solution);
			//printSolution(solution);
			//system("pause");
		}
		tmp++;
		updatePheromones(solutions, dataset, bestSolution, bestFlag);
		cout << iteration++ << " optimalValue: " << bestSolution.cost << endl;
	}
	printSolution(bestSolution);
	cout << "bestIteration: " << bestIteration << endl;
	cout << "passingTime:" << chrono::duration_cast<chrono::milliseconds>(bestTime - startTime).count() << endl << endl;
	return bestSolution;
}