#pragma once

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>

#include "structure.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

using namespace std;
using namespace rapidjson;

double random0to1();

int random0toN(int N, int mode = 1);

void printSolution(Solution& s);

void printPoint(Point& p);

float calculateDis(Point& p1, Point& p2, string mode = "MAN");

float calculateDisSmart(Point& p1, Point& p2, int** map, vector<Point>& v);

bool operator < (const Solution& s1, const Solution& s2);

bool operator < (const Point& p1, const Point& p2);

bool checkValidity(const Solution& s);

void generateJSON(Solution& bestSolution, string datasetName, string algorithm);

Dataset transferMap(Dataset& dataset);

void coord2lnglat(Solution& solution, Dataset& dataset);

MapStruct getRestrictedArea(string resArea = "setRes");

bool checkMap(const Dataset& dataset, int** map);

double searchRoute(Point p1, Point p2, int** map, vector<Point>& v);

Solution joinInter(Solution& solution, Dataset& dataset);

void calculateCargo(Solution& s);