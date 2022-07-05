#include "utils.h"

double random0to1() {
    //return rand() % 1000 / (double)1000;
    return (double)rand() / (double)RAND_MAX;
}

int random0toN(int N, int mode) {
    //srand(time(0));
    //����0��N-1�������
    if (mode == 0) return rand() % N;
    //����1��N-2�������
    if (mode == 1) return rand() % (N - 2) + 1;
    //����1��N����
    if (mode == 2) return rand() % N + 1;
}

//���Solution����С�ںű�������
bool operator < (const Solution& s1, const Solution& s2) {
    return s1.cost < s2.cost;
}

//���Point����С�ںű�������
bool operator < (const Point& p1, const Point& p2) {
    return p1.demand < p2.demand;
}

void printSolution(Solution& s) {
    int totalDemand = 0;
    
    for (int i = 0; i < s.routes.size(); i++) {
        cout << endl << "route:" << i << endl << "routeCost: " << s.routes[i].cost << endl;
        int tmpDemand = 0;
        for (int j = 0; j < s.routes[i].points.size(); j++) {
            Point& p = s.routes[i].points[j];
            cout << j << " " << p.index << " " << p.x << " " << p.y << endl << "demand: " << p.demand << " type: " << p.type << endl;
            tmpDemand += p.demand;
        }
        totalDemand += tmpDemand;
        cout << "totaldemand: " << tmpDemand << " cargo: " << s.routes[i].totalCargo << endl;
    }
    
    cout << "�����cost��" << s.cost << endl;
    cout << "�����demand��" << totalDemand << endl;
    cout << "���·������" << s.routes.size() << endl << endl;
}

void calculateCargo(Solution& s) {
    int totalCargo = 0;
    for (int i = 0; i < s.routes.size(); i++) {
        int tmpCargo = 0;
        for (int j = 0; j < s.routes[i].points.size(); j++) {
            Point& p = s.routes[i].points[j];
            tmpCargo += p.demand;
        }
        totalCargo += tmpCargo;
        s.routes[i].totalCargo = tmpCargo;
    }
}

void printPoint(Point& p) {
    cout << p.index << " x: " << p.x << " y: " << p.y << endl;
}

//����������/ŷ�Ͼ���
float calculateDis(Point& p1, Point& p2, string mode) {
    //�����پ���
    if (mode == "MAN") return abs(p1.x - p2.x) + abs(p1.y - p2.y);
    //ŷ�Ͼ���
    else return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

bool checkValidity(const Solution& s) {
    for (int i = 0; i < s.routes.size(); i++) if (s.routes[i].totalCargo > 100) return false;
    return true;
}

void generateJSON(Solution& bestSolution, string datasetName, string algorithm) {
    Document d;
    d.SetObject();
    wstring wsValue;
    Document::AllocatorType& allocator = d.GetAllocator(); // ��ȡ������
    d.AddMember("cost", bestSolution.cost, allocator);
    //��ӽ�����
    Value resAreaArray(kArrayType);
    for (int i = 0; i < bestSolution.restrictedAreaNum; i++) {
        Value pointArray(kArrayType);
        for (int j = 0; j < 2; j++) {
            Value pointObj(kObjectType);
            pointObj.AddMember("ing", bestSolution.restrictedArea[i][j][0], allocator);
            pointObj.AddMember("lat", bestSolution.restrictedArea[i][j][1], allocator);
            pointArray.PushBack(pointObj, allocator);
        }
        resAreaArray.PushBack(pointArray, allocator);
    }
    d.AddMember("resAreas", resAreaArray, allocator);
    //���route
    Value routeArray(kArrayType);
    for (int i = 0; i < bestSolution.routes.size(); i++) {
        Value pointArray(kArrayType);
        for (int j = 0; j < bestSolution.routes[i].points.size(); j++) {
            Point p = bestSolution.routes[i].points[j];
            Value pointObj(kObjectType);
            pointObj.AddMember("ing", p.ing, allocator);
            pointObj.AddMember("lat", p.lat, allocator);
            pointObj.AddMember("index", p.index, allocator);
            pointObj.AddMember("demand", p.demand, allocator);
            pointObj.AddMember("type", p.type, allocator);
            pointArray.PushBack(pointObj, allocator);
        }
        routeArray.PushBack(pointArray, allocator);
    }
    d.AddMember("routes", routeArray, allocator);
    // StringBuffer��һ���򵥵��������������һ���ڴ滺��������д������JSON��
    // ��ʹ��GetString()����ȡ�û�������
    StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);
    d.Accept(writer);

    string jsonData = strBuf.GetString();
    //cout << jsonData << endl << endl;
    //д�ļ�
    ofstream ofs;
    string filePath = "C:\\Users\\12537\\Desktop\\vrp\\solutions\\";
    string fileName = filePath + datasetName.substr(0, datasetName.size() - 4) + "-" + algorithm + ".json";
    ofs.open(fileName, ios::out);
    ofs << jsonData;
}

//����ӳ�䵽20*20������
Dataset transferMap(Dataset& dataset) {
    Dataset newDataset(dataset);
    int step = 5;
    int gap = step / 2;
    int map[25][25] = {0};
    //newDataset.points.resize(0);
    vector<Point> points;
    for (int i = 0; i < dataset.pointNum; i++) {
        Point& p = dataset.points[i];
        int x = p.x / step;
        if (p.x % step > gap) x++;
        int y = p.y / step;
        if (p.y % step > gap) y++;
        //�����������
        if (i == 0) points.push_back(Point(0, x * 5, y * 5, x, y, 0));
        else {
            //if (map[x][y]) cout << x << " " << y << endl << "debug: " << map[x][y] << " " << p.demand << endl;
            map[x][y] += p.demand;
        }
    }
    int index = 1;
    for (int i = 0; i <= 20; i++) {
        for (int j = 0; j <= 20; j++) {
            //Point p(index, i, j);
            //p.demand = map[i][j];
            if (map[i][j] > 0) {
                points.push_back(Point(index++, i * 5, j * 5, i, j, map[i][j]));
                //cout << i << " " << j << endl << "debug: " << map[i][j] << endl;
            }
        }
        //cout << endl;
    }
    //cout << "num: " << index << endl;
    newDataset.points = points;
    newDataset.pointNum = points.size();
    //��ӵ��type��0Ϊ��㣬1Ϊ�ֿ⣬2Ϊ�м��
    newDataset.points[0].type = 0;
    for (int i = 1; i < newDataset.points.size(); i++) {
        newDataset.points[i].type = 1;
    }
    //for (auto i = newDataset.points.begin(); i != newDataset.points.end(); i++)
    //    cout << (*i).index << " " << (*i).tx << " " << (*i).ty << " " << (*i).demand << endl;
    //system("pause");
    return newDataset;
}

//��solution�е�����ת��Ϊ��γ��
void coord2lnglat(Solution& solution, Dataset& dataset) {
    Solution& newSolution = solution;
    double corner1[2] = { 116.286364, 39.835336 };
    double corner2[2] = { 116.486178, 39.986495 };
    double step_x = abs(corner2[0] - corner1[0]) / (double)20;
    double step_y = abs(corner2[1] - corner1[1]) / (double)20;
    //ת��������������
    for (int i = 0; i < solution.restrictedAreaNum; i++) {
        for (int j = 0; j < 2; j++) {
            solution.restrictedArea[i][j][0] = corner1[0] + step_x * (solution.restrictedArea[i][j][0]);
            solution.restrictedArea[i][j][1] = corner1[1] + step_y * (solution.restrictedArea[i][j][1]);
            //cout << "��������γ�ȣ� " << endl;
            //cout << 
        }
    }
    //ת��·���ϵ������
    for (int i = 0; i < newSolution.routes.size(); i++) {
        vector<Point>& ps = newSolution.routes[i].points;
        for (int j = 0; j < ps.size(); j++) {
            Point& p = ps[j];
            p.ing = corner1[0] + step_x * (p.x / 5);
            p.lat = corner1[1] + step_y * (p.y / 5);
        }
        /*
        //�����м��
        vector<Point> midps;
        for (int j = 0; j < ps.size() - 1; j++) {
            int s1 = j, s2 = j + 1;
            if (rand() % 2) {
                swap(s1, s2);
            }
            Point p(-1, ps[s1].x, ps[s2].y, 0);
            p.ing = ps[s1].ing;
            p.lat = ps[s2].lat;
            midps.push_back(p);
        }
        
        vector<Point> finalps;
        for (int j = 0; j < ps.size() - 1; j++) {
            finalps.push_back(Point(ps[j]));
            finalps.push_back(Point(midps[j]));
        }
        finalps.push_back(Point(ps[ps.size() - 1]));
        ps = finalps;
        */
    }
    //return newSolution;
}


//�ƿ����������ܼ������
double searchRoute(Point p1, Point p2, int** map, vector<Point>& v) {
    //����x,y�仯�ķ���
    int dirx = (p1.tx < p2.tx) ? 1 : -1;
    int diry = (p1.ty < p2.ty) ? 1 : -1;
    int capx = abs(p1.tx - p2.tx);
    int capy = abs(p1.ty - p2.ty);
    int x = p1.tx, y = p1.ty;
    double cost = 0;
    //double cost2 = 0;
    //vector<Point> v1;
    //vector<Point> v2;
    //ͳһ����x���ϵ��ƶ�
    for (int i = 0; i < capx; i++) {
        //cout << "x: " << x << " y: " << y << endl;
        if (map[x][y] == 1 && map[x + dirx][y] == 2) {
            //��ʱ��Ҫת��
            //cout << "����=1����һ��=2: x: " << x << " y: " << y << endl;
            //cout << "debug: x: " << x << " y: " << y << endl;
            v.push_back(Point(5 * x, 5 * y));
            while (map[x][y + diry] != 0) {
                y += diry;
                cost++;
            }
            //cout << "debug: x: " << x << " y: " << y << endl;
            v.push_back(Point(5 * x, 5 * y));
            while (map[x + dirx][y] != 0) {
                //Ϊ��ֹ��Զ·�����Ѿ��ﵽp2�ĺ����꣬��ֱ��ת��
                if (x == p2.tx) {
                    v.push_back(Point(5 * x, 5 * y));
                    cost += searchRoute(Point(5 * x, 5 * y), p2, map, v);
                    return cost;
                }
                x += dirx;
                cost++;
            }
            //cout << "debug: x: " << x << " y: " << y << endl;
            v.push_back(Point(5 * x, 5 * y));
            cost += searchRoute(Point(5 * x, 5 * y), p2, map, v);
            return cost;
        }
        else {
            x += dirx;
            cost++;
        }
    }
    //cout << "debug: x: " << x << " y: " << y << endl;
    v.push_back(Point(5 * x, 5 * y));
    //printPoint(v.back());
    for (int i = 0; i < capy; i++) {
        if (map[x][y] == 1 && map[x][y + diry] == 2) {
            //cout << "����=1����һ��=2: x: " << x << " y: " << y << endl;

            //��ʱ��Ҫת��
            //���ڶ��γ��ֽ�����ʱ��������һЩ�Ż�����Լ·����
            //��x���������ƶ������ƣ�����������ڵ�ͼ��̫�����Ų������ܵ������
            //v.push_back(Point(5 * x, 5 * y));
            // 
            //���˳���
            int traceback = 0;
            int tmpx = x;
            double tmpCost = cost;
            while (map[tmpx - dirx][y] != 0) {
                tmpx -= dirx;
                traceback++;
                tmpCost--;
            }
            if (traceback <= capx) {
                x = tmpx;
                cost = tmpCost;
                Point tmp(5 * x, 5 * p1.ty);
                v.pop_back();
                v.push_back(tmp);
                //v.push_back(Point(tmp.x, p2.ty));
                //cout << "inner cost: " << cost << " rest: " << calculateDis(tmp, p2) << endl;
                //cost += calculateDis(tmp, p2);
                while (map[x][y + diry] != 0) {
                    y += diry;
                    cost++;
                }
                v.push_back(Point(5 * x, 5 * y));
                cost += searchRoute(Point(5 * x, 5 * y), p2, map, v);
                return cost;
            }
            else {
                v.push_back(Point(5 * x, 5 * y));
                //ע�����ﻻ���˼���
                while (map[x - dirx][y] != 0) {
                    x -= dirx;
                    cost++;
                }
                //cout << "debug: x: " << x << " y: " << y << endl;
                v.push_back(Point(5 * x, 5 * y));
                while (map[x][y + diry] != 0) {
                    y += diry;
                    cost++;
                }
                //cout << "debug: x: " << x << " y: " << y << endl << "inter: " << endl;
                for (auto i : v) 
                    //cout << i.tx << " " << i.ty << endl;
                v.push_back(Point(5 * x, 5 * y));
                cost += searchRoute(Point(5 * x, 5 * y), p2, map, v);
                return cost;
            }
        }
        else {
            y += diry;
            cost++;
            //cout << "hhh: " << cost << endl;
        }
    }
    //ͳһ�����м���type��index��demand����Ϣ
    for (int i = 0; i < v.size(); i++) {
        v[i].type = 2;
        v[i].index = -1;
        v[i].demand = 0;
    }
    return cost;
}
/*
float calculateDisSmart(Point& p1, Point& p2, int** map, vector<Point>& v) {
    float dis = 0;
    return (p1, p2, map, v);
    
    //while (x != p2.x && y != p2.y) {

    //}
    //�����м��
    
    vector<Point> midps;
    for (int j = 0; j < ps.size() - 1; j++) {
        int s1 = j, s2 = j + 1;
        if (rand() % 2) {
            swap(s1, s2);
        }
        Point p(-1, ps[s1].x, ps[s2].y, 0);
        p.ing = ps[s1].ing;
        p.lat = ps[s2].lat;
        midps.push_back(p);
    }
    return dis;
}
*/

//���������
//������ͨ���Ľڵ���Ϊ1
void markArea(MapStruct& mapStruct, int index, int x, int y, int w, int h) {
    int** map = mapStruct.map;
    for (int i = 0; i <= w; i++)
        for (int j = 0; j <= h; j++) {
            map[x + i][y + j] = 1;
        }
    for (int i = 1; i < w; i++)
        for (int j = 1; j < h; j++) {
            map[x + i][y + j] = 2;
        }
    //cout << "index: " << index << endl;
    mapStruct.restrictedArea[index][0][0] = x;
    mapStruct.restrictedArea[index][0][1] = y;    
    mapStruct.restrictedArea[index][1][0] = x + w;
    mapStruct.restrictedArea[index][1][1] = y + h;
}

MapStruct getRestrictedArea(string resArea) {
    MapStruct mapStruct;
    int** map;
    map = new int*[25];
    for (int i = 0; i < 25; i++) map[i] = new int[25];
    for (int i = 0; i < 25; i++)
        for (int j = 0; j < 25; j++) map[i][j] = 0;
    mapStruct.map = map;
    mapStruct.restrictedAreaNum = 4;
    int index = 0;
    if (resArea == "setRes") {
        markArea(mapStruct, index++, 9, 13, 2, 2);
        markArea(mapStruct, index++, 3, 7, 2, 2);
        markArea(mapStruct, index++, 15, 9, 2, 2);
        markArea(mapStruct, index++, 13, 5, 2, 2);
    }
    return mapStruct;
}

bool checkMap(const Dataset& dataset, int** map) {
    for (int i = 0; i < dataset.points.size(); i++) {
        int x = dataset.points[i].x / 5;
        int y = dataset.points[i].y / 5;
        if (map[x][y] == 2) {
            cout << "map���������Ϸ�������������" << endl;
            cout << "�Ƿ���������x=" << x << " y=" << y << endl;
            return false;
        }
    }
    return true;
}

//��������inter����
Solution joinInter(Solution& solution, Dataset& dataset) {
    Solution newSolution(solution);
    for (int i = 0; i < newSolution.routes.size(); i++) {
        vector<Point>& ps = newSolution.routes[i].points;
        vector<Point> finalps;
        //��������յ㣬���ﶼ��dataset�е�points[0]
        ps.insert(ps.begin(), dataset.points[0]);
        ps.push_back(dataset.points[0]);
        for (int j = 0; j < ps.size() - 1; j++) {
            int index1 = ps[j].index;
            int index2 = ps[j + 1].index;
            int sign = index1 > index2 ? 1 : 0;
            vector<Point> interps = Solution::inter[min(index1, index2)][max(index1, index2)];
            /*
            cout << "x1: " << ps[j].x << " y1: " << ps[j].y << endl
                << "x2: " << ps[j + 1].x << " y2: " << ps[j + 1].y << endl;
            cout << "interps:" << endl;
            //for (int k = 0; k < interps.size(); k++) cout << "x: " << interps[k].x << " y: " << interps[k].y << endl;
            */
            //inter��ֻ����i<j���ε�inter��������Ҫ��תһ��
            if (sign) reverse(interps.begin(), interps.end());
            finalps.push_back(ps[j]);
            //�����м��
            for (int k = 0; k < interps.size(); k++) {
                Point& p = interps[k];
                //ȥ��
                if (p.x != finalps.back().x || p.y != finalps.back().y) finalps.push_back(p);
            }
        }
        //system("pause");
        //�������һ����
        finalps.push_back(ps.back());
        //for (int k = 0; k < finalps.size(); k++) cout << "x: " << finalps[k].x << " y: " << finalps[k].y << endl;
        ps = finalps;
    }
    return newSolution;
}