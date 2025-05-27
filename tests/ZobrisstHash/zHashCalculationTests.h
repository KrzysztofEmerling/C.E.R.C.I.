
#include <iostream>
#include <chrono>
#include "BoardUtils.h"
#define DISABLE_REFZHASH_WARN

#pragma once

using namespace std::chrono;

class zHashCalculationTests
{
public:
    static void RunLongTests();
    static void RunQuickTests();

private:
    u64 PerftWithZHashCheck(BoardState &state, int depth);
};
