
#include <iostream>
#include <chrono>
#include "BoardState.h"

#pragma once

using namespace std::chrono;

class zHashCalculationTests
{
public:
    static void RunTests();

private:
    static u64 PerftWithZHashCheck(BoardState &state, int depth);
};
