#pragma once
#include "structure.h"
#include "utils.h"
#include <ctime>
#include <iostream>
#include <vector>
#include <chrono>

class SA
{
	public:
		static Solution run(Dataset& dataset, int** map, double T0 = 0.1, double coolingRate = 0.9, int timeLimit = 20);
};

