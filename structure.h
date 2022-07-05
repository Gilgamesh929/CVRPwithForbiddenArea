#pragma once
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

class Point {
public:
    int x;
    int y;
    //tiny x tiny y是x和y除以5
    int tx;
    int ty;
    int index;
    int demand;
    //类型：0为仓库，1为起点，2为终点，3为可通过中心点，4为不可通过中心点
    int type;
    float prevCost;
    //经纬度
    double ing;
    double lat;


    Point(int i, int xx, int yy, int c = 0, int t = 0);
    Point(int i, int xx, int yy, int ttx, int tty, int c = 0, int t = 0);
    Point(int xx, int yy);
};

class Station {
public:
    Point point; 
    int timestamp;
};

class Route {
public:
    //vector<Station> stations;
    vector<Point> points;
    double cost;
    int totalCargo;
    //空间利用率
    double utiRate;

    float calculateCost();
};

struct MapStruct {
    int** map;
    int restrictedAreaNum;
    double restrictedArea[20][2][2];
};

class Solution {
public:
    unsigned code;
    vector<Route> routes;
    double cost;
    //两个点之间的inter正向反向只存一个，另一个通过reverse得到
    static vector<Point> inter[100][100];
    //禁行区
    int restrictedAreaNum;
    double restrictedArea[20][2][2];

    //Solution();
    Solution mutate();
    float calculateCost();
    void addRestrictedArea(MapStruct mapStruct);
    //bool operator <(const Solution& s);
};

class Dataset {
public:
    int capacity;
    int truckNum;
    int pointNum;
    float optimalValue;
    vector<Point> points;

    //Dataset(int c, int t, int p, float o, vector<Point> ps);
};

//辅助函数
float calculateDis(Point* p1, Point* p2);


class Test {
public:
    static vector<Point> inter[100][100];
    //static vector<vector<vector<Point>>> inter;
};