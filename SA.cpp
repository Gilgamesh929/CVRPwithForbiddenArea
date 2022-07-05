#include "SA.h"

//using namespace std;

//两个点之间的inter正向反向只存一个，另一个通过reverse得到
//vector<Point> inter[100][100];
double dis[100][100];
double Tmax = 25;

auto startTime2 = chrono::system_clock::now();
auto bestTime2 = chrono::system_clock::now();

void calculateSolCost(Solution& solution) {
    double cost = 0;
    for (int i = 0; i < solution.routes.size(); i++) {
        double tmpCost = 0;
        vector<Point>& p = solution.routes[i].points;
        int a = 0, b = 0;
        for (int j = 0; j < p.size(); j++) {
            b = p[j].index;
            tmpCost += dis[min(a, b)][max(a, b)];
            //cout << "i1: " << a << " i2: " << b << " cost: " << dis[min(a, b)][max(a, b)] << " totalCost: " << cost << endl;
            a = b;
        }
        //注意这里起点终点都默认是p[0]
        b = 0;
        tmpCost += dis[min(a, b)][max(a, b)];
        solution.routes[i].cost = tmpCost;
        cost += tmpCost;
        //计算空间利用率
        solution.routes[i].utiRate = (double)solution.routes[i].totalCargo / tmpCost;
        //cout << "i1: " << a << " i2: " << b << " cost: " << dis[min(a, b)][max(a, b)] << " totalCost: " << cost << endl;
    }
    solution.cost = cost;
}

void combineRoutes(Solution& s, int sign1, int sign2, float benefit) {
    Route& r1 = s.routes[sign1];
    Route& r2 = s.routes[sign2];
    //更新点的prevCost
    //r2.points[1].prevCost = calculateDis(r1.points[r1.points.size() - 2], r2.points[1]);
    //r1.points.pop_back();
    for (int i = 0; i < r2.points.size(); i++) {
        r1.points.push_back(r2.points[i]);
    }
    r1.totalCargo += r2.totalCargo;
    if (r1.totalCargo > 100) { cout << "??? :" << r1.totalCargo << endl; system("pause"); }
    //r1.cost = r1.cost + r2.cost - benefit;
    //cout << " sign1: " << sign1 << " sign2: " << sign2 << endl;
    auto it = s.routes.begin() + sign2;
    s.routes.erase(it);
}

void constructInitSol(Solution& initSolution, Dataset& dataset) {
    auto p = dataset.points;
    //Solution initSolution;
    Point start = p[0];
    Point end = p[0];
    //先给每一个仓库分配一条路径
    for (auto it = p.begin() + 1; it != p.end(); it++) {
        Route tmpRoute;
        //tmpRoute.points.push_back(start);
        //(*it).prevCost = calculateDis(start, *it);
        tmpRoute.points.push_back(*it);
        //end.prevCost = calculateDis(*it, end);
        //tmpRoute.points.push_back(end);
        //tmpRoute.cost = tmpRoute.calculateCost();
        tmpRoute.totalCargo = (*it).demand;
        initSolution.routes.push_back(tmpRoute);
    }
    //printSolution(initSolution);
    //system("pause");
    //step1:使用savings algorithm（Wright&Clarke 1964）优化初始解，同时减少路径数目
    while (1) {
        int routeNum = initSolution.routes.size();
        float maxBenefit = 0;
        int sign1 = -1, sign2 = -1;
        for (int i = 0; i < routeNum; i++)
            for (int j = 0; j < routeNum; j++) {
                if (i == j) continue;
                Route& r1 = initSolution.routes[i];
                Route& r2 = initSolution.routes[j];
                int p1 = r1.points[r1.points.size() - 1].index;
                int p2 = r2.points[0].index;
                //float benefit = calculateDis(p1, end, "MAN") + calculateDis(start, p2, "MAN") - calculateDis(p1, p2, "MAN");
                float benefit = dis[min(p1, end.index)][max(p1, end.index)] + dis[min(p2, start.index)][max(p2, start.index)] - dis[min(p1, p2)][max(p1, p2)];
                //注意车辆容量
                if (benefit > maxBenefit && r1.totalCargo + r2.totalCargo <= dataset.capacity) {
                    maxBenefit = benefit;
                    sign1 = i; sign2 = j;
                }
            }
        //cout << "sign1:" << sign1 << "  sign2:" << sign2 << "  benefit:" << maxBenefit << endl;
        //if (sign1 >= 0 && sign2 >= 0) cout << "cargo1:" << initSolution.routes[sign1].totalCargo << "  cargo2:" << initSolution.routes[sign2].totalCargo << " sum: " << initSolution.routes[sign1].totalCargo + initSolution.routes[sign2].totalCargo << endl;
        if (maxBenefit > 0) combineRoutes(initSolution, sign1, sign2, maxBenefit);
        else break;
    }
    //计算解的总cost和cargo
    calculateSolCost(initSolution);
    calculateCargo(initSolution);
    //return;
    //step2:若路径数目仍超过车辆数目，继续减少路径数目

    //if (initSolution.routes.size() > dataset.truckNum) { cout << "需要进行第二步" << endl << "step1 cost: " << initSolution.cost << endl; system("pause"); }
    //printSolution(initSolution);
    while (initSolution.routes.size() > dataset.truckNum) {
        //找到空间利用率最低的路径
        double minUti = initSolution.routes[0].utiRate;
        int flag = 0;
        for (int i = 1; i < initSolution.routes.size(); i++) {
            if (initSolution.routes[i].utiRate < minUti) {
                minUti = initSolution.routes[i].utiRate;
                flag = i;
            }
        }
        //cout << "flag: " << flag << endl;
        vector<Point> unserved;
        for (int i = 0; i < initSolution.routes[flag].points.size(); i++) {
            unserved.push_back(Point(initSolution.routes[flag].points[i]));
        }
        initSolution.routes.erase(initSolution.routes.begin() + flag);
        //按demand升序排序，小于号已重载
        sort(unserved.begin(), unserved.end());
        while (!unserved.empty()) {
            double minCost = 0;
            int in = unserved.back().index;
            int rflag = -1, pflag = 0;
            for (int i = 1; i < initSolution.routes.size(); i++) {
                Route& r = initSolution.routes[i];
                if (dataset.capacity - r.totalCargo < unserved.back().demand) continue;
                //将初始rflag设为第一个符合容量限制的路径
                if (rflag == -1) rflag = i;
                int a = 0, b = 0; 
                for (int j = 0; j < r.points.size(); j++) {
                    Point& p = r.points[j];
                    b = p.index;
                    float tmpCost = dis[min(a, in)][max(a, in)] + dis[min(in, b)][max(in, b)];
                    if (minCost == 0 || minCost > tmpCost) {
                        rflag = i;
                        pflag = j;
                        minCost = tmpCost;
                    }
                    a = b;
                }
                b = 0;
                float tmpCost = dis[min(a, in)][max(a, in)] + dis[min(in, b)][max(in, b)];
                if (minCost > tmpCost) {
                    rflag = i;
                    pflag = r.points.size() - 1;
                    minCost = tmpCost;
                }
            }
            /*
            cout << "Point: " << endl;
            printPoint(unserved.back());
            cout << "插入位置: " << " r: " << rflag << " p: " << pflag << endl;
            */
            //当找不到插入点时，随机选择一条路径，将其中的点放入unserved直到当前点可插入该路径
            if (rflag == -1) {
                //当无法满足限制时，选中的逐渐移除点的路径下标
                int rindex = rand() % initSolution.routes.size();
                //cout << "debug index: " << rindex << endl;
                Route& tr = initSolution.routes[rindex];
                while (dataset.capacity - tr.totalCargo < unserved.back().demand) {
                    unserved.insert(unserved.begin(), tr.points.back());
                    tr.totalCargo -= tr.points.back().demand;
                    tr.points.pop_back();
                    //cout << "debug demand: " << tr.totalCargo << endl;
                }
                //找到tr上最合适的插入点
                minCost = 0;
                int a = 0, b = 0;
                for (int j = 0; j < tr.points.size(); j++) {
                    Point& p = tr.points[j];
                    b = p.index;
                    float tmpCost = dis[min(a, in)][max(a, in)] + dis[min(in, b)][max(in, b)];
                    if (minCost == 0 || minCost > tmpCost) {
                        pflag = j;
                        minCost = tmpCost;
                    }
                    a = b;
                }
                b = 0;
                float tmpCost = dis[min(a, in)][max(a, in)] + dis[min(in, b)][max(in, b)];
                if (minCost > tmpCost) {
                    pflag = tr.points.size() - 1;
                    minCost = tmpCost;
                }
                //插入当前点
                initSolution.routes[rindex].points.insert(initSolution.routes[rindex].points.begin() + pflag, unserved.back());
                unserved.pop_back();
                //更新cost和cargo
                calculateSolCost(initSolution);
                calculateCargo(initSolution);
                continue;
            }
            initSolution.routes[rflag].points.insert(initSolution.routes[rflag].points.begin() + pflag, unserved.back());
            unserved.pop_back();
            //更新cost和cargo
            calculateSolCost(initSolution);
            calculateCargo(initSolution);
        }
    }
    //return initSolution;
}


Solution SA::run(Dataset& dataset, int** map, double T0, double coolingRate, int timeLimit) {
    srand(time(0));
    Solution initSolution;
    //生成边的距离
    startTime2 = chrono::system_clock::now();
    for (int i = 0; i < dataset.pointNum; i++)
        for (int j = i + 1; j < dataset.pointNum; j++) {
            Point& p1 = dataset.points[i];
            Point& p2 = dataset.points[j];
            //dis[i][j] = calculateDis(p1, p2);
            vector<Point> v;
            //cout << " x1: " << p1.tx << " y1: " << p1.ty << " x2: " << p2.tx << " y2: " << p2.ty << endl;
            dis[i][j] = 5 * searchRoute(p1, p2, map, v);
            Solution::inter[p1.index][p2.index] = v;
            //dis[i][j] = calculateDis(p1, p2, "EUC");
            /*
            cout << " x1: " << p1.tx << " y1: " << p1.ty
                << " x2: " << p2.tx << " y2: " << p2.ty
                << " dis: " << dis[i][j] << " man: " << calculateDis(p1, p2) << endl << "inter vector: " << endl;
            for (auto a : initSolution::inter[p1.index][p2.index]) {
                cout << a.tx << " " << a.ty << endl;
            }
            if (calculateDis(p1, p2) != dis[i][j]) 
            ("pause");
            */
        }
    //system("pause");
	//构建初始解
	constructInitSol(initSolution, dataset);
    //测试节约算法
    //bestTime2 = chrono::system_clock::now();
    //printSolution(initSolution); system("pause");
    //cout << "passingTime:" << chrono::duration_cast<std::chrono::milliseconds>(bestTime2 - startTime2).count() << std::endl;
    //return initSolution;
    //printSolution(initSolution);
    //auto newSolution = initSolution.mutate();
    //printSolution(newSolution);
    //return initSolution;
    
    Solution curSolution = initSolution;
    Solution bestSolution = initSolution;
	double T = T0;
	double Tb = T0;//最优解时的温度
	time_t startTime = time(nullptr);
    //模拟退火
    int flag = 0;
	while (time(nullptr) - startTime <= timeLimit)
	{
		Solution nextSolution = curSolution.mutate();
        //需要保证生成的解是合法的
        while (! checkValidity(nextSolution)) nextSolution = curSolution.mutate();
        calculateSolCost(nextSolution);
        cout << flag++ << " T: " << T << " best: " << bestSolution.cost << " cur: " << curSolution.cost << " next: " << nextSolution.cost << endl;
		double delta = (double)nextSolution.cost - (double)curSolution.cost;
        //cout << "p: " << exp(-delta / T) << endl; system("pause");
        double randomVal = random0to1();
		if (delta <= 0 || exp(-(double)delta / T) > randomVal)
		{
            if (delta == 0) { 
                //printSolution(curSolution); printSolution(nextSolution); 
                //("pause"); 
            }
            //printSolution(nextSolution);
            //cout << "delta: " << delta << " T: " << T <<  " p: " << exp(-delta / T) << " rand: " << randomVal << endl; system("pause");
            //system("pause");
            //cout << "SA -> Next solution: " << candidate->weight << ", T: " << current_temp << ", " << "Best: " << best_solution->weight << ", Delta: " << delta << "\n";
            curSolution = nextSolution;
            if (bestSolution.cost > curSolution.cost) {
                bestTime2 = chrono::system_clock::now();
				bestSolution = curSolution;
                //printSolution(bestSolution);
                //system("pause");
                Tb = T;
			}
		}
		T *= coolingRate;
		//std::cout << "T: " << current_temp << "\n";
        if (T < 0.01) {
            Tb *= 2;
            T = min(Tb, Tmax);
        }
	}
    printSolution(bestSolution);
    cout << "passingTime:" << chrono::duration_cast<chrono::milliseconds>(bestTime2 - startTime2).count() << endl << endl;
    return bestSolution;
}