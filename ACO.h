#pragma once

#include <chrono>
#include <iostream>
#include <array>
#include <cmath>
#include <random>
#include "utils.h"
#include "structure.h"

class ACO
{
public:
		static Solution run(Dataset& dataset, int** map, int timeLimit, int antNum = 25, double inputRo = 0.85, int inputSigma = 3);
};

