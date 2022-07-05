// vrp_demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "structure.h"
#include "SA.h"
#include "ACO.h"
#include "data.h"

vector<Point> Solution::inter[100][100];

vector<string> data = {"A-n32-k5", "A-n33-k5", "A-n33-k6", "A-n34-k5", "A-n36-k5", "A-n37-k5", "A-n37-k6", "A-n38-k5", "A-n39-k5", "A-n39-k6", "A-n44-k6", 
"A-n45-k6", "A-n45-k7", "A-n46-k7", "A-n48-k7", "A-n53-k7", "A-n54-k7", "A-n55-k9", "A-n60-k9", "A-n61-k9", "A-n62-k8", "A-n63-k9", "A-n63-k10", "A-n64-k9", 
"A-n65-k9", "A-n69-k9", "A-n80-k10"};

int main(int argc, char* argv[])
{
    //解析命令行参数
    string datasetName = argv[1];
    string resArea = argv[2];
    int timeLimit = stoi(argv[3]);
    string algorithm = argv[4];
    //cout << "dataset: " << datasetName << " time: " << timeLimit << " algorithm: " << algorithm << endl; return 0;
    //参数：蚁群规模、精英蚂蚁数目、初始温度
    int antNum = 25, sigma = 3;
    //参数：蒸发速率、冷却速率
    double ro = 0.85, coolingRate = 0.9, T0 = 0.1;
    if (algorithm == "SA") {
        T0 = stod(argv[5]);
        coolingRate = stod(argv[6]);
        //cout << "T0: " << T0 << " coolingRate: " << coolingRate << endl;
    }
    else if (algorithm == "ACO") {
        antNum = stoi(argv[5]);
        ro = stod(argv[6]);
        sigma = stoi(argv[7]);
        //cout << "antNum: " << antNum << " ro: " << ro << " sigma: " << sigma << endl; return 0;
    }
    else {
        cout << "请输入正确的算法" << endl;
        return 0;
    }
    //string datasetName = "A-n34-k5.vrp";
    Dataset dataset;
    //读入数据集
    readDataset(datasetName, dataset);
    //auto newDataset = dataset;
    auto newDataset = transferMap(dataset);
    //for (int i = 0; i < newDataset.pointNum; i++) cout << newDataset.points[i].index << " " << newDataset.points[i].x << " " << newDataset.points[i].y << " " << newDataset.points[i].demand << endl;
    //system("pause");
    //return 0;
    // 构造禁行区
    auto mapStruct = getRestrictedArea(resArea);
    auto map = mapStruct.map;
    //验证map合法性，保证没有点在禁行区
    if (!checkMap(newDataset, map)) return 0;
    Solution bestSolution;
    if (algorithm == "ACO") {
        //for(int l = 0; l < 6; l++)
        bestSolution = ACO::run(newDataset, map, timeLimit, antNum, ro, sigma);
    }
    else if (algorithm == "SA") {
        //for (int l = 0; l < 20; l++)
        bestSolution = SA::run(newDataset, map, T0, coolingRate, timeLimit);
    }
    //插入中间点序列
    auto finalSolution = joinInter(bestSolution, newDataset);
    //添加禁行区信息
    finalSolution.addRestrictedArea(mapStruct);
    printSolution(finalSolution);
    /*
    for (int i = 0; i < finalSolution.restrictedAreaNum; i++) {
        cout << "禁行区：" <<i << endl;
        cout << "x1: " << finalSolution.restrictedArea[i][0][0] << " y1: " << finalSolution.restrictedArea[i][0][1] << endl;
        cout << "x2: " << finalSolution.restrictedArea[i][1][0] << " y2: " << finalSolution.restrictedArea[i][1][1] << endl;
    }
    */
    //将坐标转换为经纬度
    coord2lnglat(finalSolution, newDataset);
    //生成JSON文件存储解决方案
    generateJSON(finalSolution, datasetName, algorithm);
    /*
    for (int i = 0; i < dataset.points.size(); i++) {
        cout << dataset.points[i].index << " " << dataset.points[i].x << " " << dataset.points[i].y << endl;
    }
    */
    return 0;
}