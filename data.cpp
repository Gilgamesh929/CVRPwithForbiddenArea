#include "data.h"

using namespace std;

void readDataset(string filename, Dataset& dataset)
{
    ifstream ifs; 
    ifs.open("C:\\Users\\12537\\Desktop\\vrp\\dataset\\A\\" + filename, ios::in);
    //ifs.open("dataset\\A\\" + filename, ios::in);
    if (!ifs.is_open()) {
        cout << "ERR|read file fail" << endl;
    }
    string buf;
    smatch result;
    while (getline(ifs, buf)) {
        if (regex_search(buf, result, regex("No of trucks: (\\d+)"))) { 
            dataset.truckNum = stoi(result.str(1)); 
            continue; 
        }
        if (regex_search(buf, result, regex("Optimal value: (\\d+)"))) { 
            dataset.optimalValue = stoi(result.str(1));
            continue; 
        }
        if (regex_search(buf, result, regex("DIMENSION : (\\d+)"))) { 
            dataset.pointNum = stoi(result.str(1));
            continue; 
        }
        if (regex_search(buf, result, regex("CAPACITY : (\\d+)"))) { 
            dataset.capacity = stoi(result.str(1));
            continue; 
        }
        if (regex_search(buf, result, regex("(\\d+)\\s(\\d+)\\s(\\d+)"))) {
            //为方便后续处理，把仓库的index减一（从0开始）
            Point p(stoi(result.str(1)) - 1, stoi(result.str(2)), stoi(result.str(3)));
            dataset.points.push_back(p);
            continue; 
        }
        if (regex_search(buf, result, regex("(\\d+)\\s(\\d+)"))) {
            int index = stoi(result.str(1)) - 1;
            dataset.points[index].demand = stoi(result.str(2));
        }
    }
    ifs.close();
    return;
}