#include "structure.h"
#include "utils.h"


Point::Point(int i, int xx, int yy, int c, int t) {
    index = i;
    x = xx;
    y = yy;
    demand = c;
    type = t;
}

Point::Point(int i, int xx, int yy, int ttx, int tty, int c, int t) {
    index = i;
    x = xx;
    y = yy;
    tx = ttx;
    ty = tty;
    demand = c;
    type = t;
}


Point::Point(int xx, int yy) {
    x = xx;
    y = yy;
    tx = xx / 5;
    ty = yy / 5;
}

/*
Dataset::Dataset(int c, int t, int p, float o, vector<Point> ps) {
    capacity = c;
    truckNum = t;
    pointNum = p;
    optimalValue = o;
    points = ps;
}
*/

float Route::calculateCost() {
    float cost = 0;
    for (int i = 1; i < this->points.size(); i++) {
        cost += this->points[i].prevCost;
    }
    this->cost = cost;
    return cost;
}

float Solution::calculateCost() {
    float cost = 0;
    for (int i = 0; i < this->routes.size(); i++) {
        cost += this->routes[i].cost;
    }
    this->cost = cost;
    return cost;
}

//应该是route的成员函数
void erasePoint(Route& route, int index) {
    vector<Point>& r = route.points;
    auto it = r.begin();
    route.totalCargo -= (*(it + index)).demand;
    r.erase(it + index);
    /*
    printPoint(r[index - 1], "beforeErasePoint:");
    printPoint(r[index], "afterErasePoint:");*/
}

void insertPoint(Route& route, int index, Point& p) {
    vector<Point>& r = route.points;
    auto it = r.begin();
    route.totalCargo += p.demand;
    r.insert(it + index, p);
    /*
    printPoint(r[index - 1], "beforeInsertPoint:");
    printPoint(r[index], "insertPoint:");
    printPoint(r[index + 1], "afterInsertPoint:");*/
}

void Solution::addRestrictedArea(MapStruct mapStruct) {
    for (int i = 0; i < mapStruct.restrictedAreaNum; i++) {
        restrictedArea[i][0][0] = mapStruct.restrictedArea[i][0][0];
        restrictedArea[i][0][1] = mapStruct.restrictedArea[i][0][1];
        restrictedArea[i][1][0] = mapStruct.restrictedArea[i][1][0];
        restrictedArea[i][1][1] = mapStruct.restrictedArea[i][1][1];
    }
    restrictedAreaNum = mapStruct.restrictedAreaNum;
}


Solution Solution::mutate() {
    //设置随机数种子
    //srand(time(0));
    Solution newSolution(*this);
    /*
    for (int i = 0; i < 10; i++) {
        cout << random0toN(5) << endl;
    }
    
    ("pause");
    */
    if (this->routes.size() < 2) cout << "ERR|路径数小于2" << endl;
    int mutateIndex = random0toN(2, 0);
    //debug
    //int mutateIndex = 0;
    //cout << "变异算子：" << mutateIndex << endl;


    //变异算子1：一个节点移动到另一位置上
    if (mutateIndex == 0) {
        //注意while容易死循环，注意随机数函数的mode参数
        int routesNum = newSolution.routes.size();
        //cout << "路径总数： " << routesNum << endl;
        //cout << "random test: " << endl;
        //for (int i = 0; i < 8; i++) cout << random0toN(routesNum, 0) << endl;
        int routeIndex1 = random0toN(routesNum, 0);
        while (newSolution.routes[routeIndex1].points.size() <= 1) {
            routeIndex1 = random0toN(routesNum, 0);
            //cout << "死循环位置1" << endl;
        }
        int routeIndex2 = random0toN(routesNum, 0);
        int a = 0;
        /*
        while (routeIndex1 == routeIndex2) {
            routeIndex2 = random0toN(routesNum, 0);
            cout << "死循环位置2" << endl;
            a++;
            if (a >= 6) { 
                //printSolution(newSolution);
                cout << "routeIndex11: " << routeIndex1 << "  routeIndex2: " << routeIndex2 << endl;
                system("pause");
            }
        }*/
        //cout << "选择的路1：" << routeIndex1 << endl;
        //cout << "选择的路2：" << routeIndex2 << endl;
        int length1 = newSolution.routes[routeIndex1].points.size();
        int length2 = newSolution.routes[routeIndex2].points.size();
        while (routeIndex1 == routeIndex2 && length1 == 1) routeIndex2 = random0toN(routesNum, 0);
        int pointIndex1 = random0toN(length1, 0);
        int pointIndex2 = random0toN(length2, 0);
        //不同路径的情况
        if (routeIndex1 != routeIndex2) {
            //cout << "选择的点1：" << routeIndex1 << " " << pointIndex1 << endl;
            //cout << "选择的点2：" << routeIndex2 << " " << pointIndex2 << endl;
            //printSolution(newSolution);
            //删除原位置的节点
            Route& route1 = newSolution.routes[routeIndex1];
            Route& route2 = newSolution.routes[routeIndex2];
            Point p = route1.points[pointIndex1];
            erasePoint(route1, pointIndex1);
            //将节点加到新的位置
            insertPoint(route2, pointIndex2, p);
        }
        //同一路径的情况
        else {
            //若该条路径只有一个点，直接返回
            if (length1 == 1) { cout << "什么鬼"; system("pause"); return newSolution; }
            while (pointIndex1 == pointIndex2) pointIndex2 = random0toN(length2, 0);
            Route& route1 = newSolution.routes[routeIndex1];
            Point p = route1.points[pointIndex1];
            //cout << "选择的点1：" << routeIndex1 << " " << pointIndex1 << endl;
            //cout << "选择的点2：" << routeIndex2 << " " << pointIndex2 << endl;
            //cout << "before mutate: " << endl; printSolution(newSolution);
            erasePoint(route1, pointIndex1);
            //将节点加到新的位置
            if (pointIndex1 > pointIndex2) insertPoint(route1, pointIndex2, p);
            else insertPoint(route1, pointIndex2 - 1, p);
            //cout << "after mutate: " << endl; printSolution(newSolution); system("pause");
        }
    }


    //变异算子2：两个节点进行交换(同路径/不同路径)
    else if (mutateIndex == 1) {
        int routesNum = newSolution.routes.size();
        int routeIndex1 = random0toN(routesNum, 0);
        int routeIndex2 = random0toN(routesNum, 0);
        int length1 = newSolution.routes[routeIndex1].points.size();
        int length2 = newSolution.routes[routeIndex2].points.size();
        int pointIndex1 = random0toN(length1, 0);
        int pointIndex2 = random0toN(length2, 0);
        
        while (routeIndex1 == routeIndex2 && pointIndex1 == pointIndex2) {
            //若选择同一条路径且路径上只有一个点
            if (length1 == 1) while (routeIndex1 == routeIndex2) routeIndex2 = random0toN(routesNum, 0);
            else pointIndex2 = random0toN(length2, 0);
            //cout << "mutate1 死循环" << routeIndex1 << " " << routeIndex2 << endl;
            //printSolution(newSolution); system("pause");
        }
        Route& route1 = newSolution.routes[routeIndex1];
        Route& route2 = newSolution.routes[routeIndex2];
        //交换两个点
        Point p1 = route1.points[pointIndex1];
        Point p2 = route2.points[pointIndex2];
        route1.totalCargo -= p1.demand - p2.demand;
        route2.totalCargo += p1.demand - p2.demand;
        route1.points[pointIndex1] = p2;
        route2.points[pointIndex2] = p1;
    }

    //变异算子3：逆转算子，同一路径上两个位置间的点reverse
    else if (mutateIndex == 2) {
        int routesNum = newSolution.routes.size();
        int routeIndex1 = random0toN(routesNum, 0);
        while (newSolution.routes[routeIndex1].points.size() < 2) routeIndex1 = random0toN(routesNum, 0);
        int length1 = newSolution.routes[routeIndex1].points.size();
        int pointIndex1 = random0toN(length1, 0);
        int pointIndex2 = random0toN(length1, 0);
        while (pointIndex1 == pointIndex2) pointIndex2 = random0toN(length1, 0);
        //if ()
    }
    //更新解的总cost
    //newSolution.calculateCost();
    //cout << endl << endl << "新解的cost" << endl << newSolution.cost<<endl << endl;
    return newSolution;
}