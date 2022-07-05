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
    //tiny x tiny y��x��y����5
    int tx;
    int ty;
    int index;
    int demand;
    //���ͣ�0Ϊ�ֿ⣬1Ϊ��㣬2Ϊ�յ㣬3Ϊ��ͨ�����ĵ㣬4Ϊ����ͨ�����ĵ�
    int type;
    float prevCost;
    //��γ��
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
    //�ռ�������
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
    //������֮���inter������ֻ��һ������һ��ͨ��reverse�õ�
    static vector<Point> inter[100][100];
    //������
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

//��������
float calculateDis(Point* p1, Point* p2);


class Test {
public:
    static vector<Point> inter[100][100];
    //static vector<vector<vector<Point>>> inter;
};