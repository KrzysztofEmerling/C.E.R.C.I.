#include <iostream>
#include <queue>
#include <vector>
#include <map>
#include <chrono>

#include "Perft.h"

#pragma once

using namespace std::chrono;

struct PerftTestCase
{
    std::string fen;
    std::map<int, u64> expectedNodes; // map zamiast unordered_map -> gwarantuje kolejność depth
};

class PerftTests
{
public:
    static void RunLongTests();
    static void RunQuickTest();
};