#include "BoardState.h"
#include <cmath>        

#pragma once

class DNN
{
public:
    static void LoadWeights(const std::string& filepath);
    static float FitForward(const BoardState &board);

private:
    static inline float elu(float x, float alpha = 1.0f) {
        return x >= 0 ? x : alpha * (std::exp(x) - 1.0f);
    }
    // l0
    static float W0[768 * 1024];
    static float b0[1024];

    // l1
    static float W1[1024 * 512];
    static float b1[512];

    // l2
    static float W2[512 * 1];
    static float b2[1];


    static void board2tensor(const BoardState &board, float (&tensor)[768]);
};
