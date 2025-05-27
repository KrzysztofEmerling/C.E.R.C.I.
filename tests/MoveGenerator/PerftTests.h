#include <iostream>
#include <chrono>

#pragma once

using namespace std::chrono;

class PerftTests
{
public:
    static void RunLongTests();
    static void RunQuickTests();
};