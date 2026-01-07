#include "BoardState.h"
#include <cmath>        

#pragma once

class DNN
{
public:
    static float FitForward(const BoardState &board);

private:
    static inline float elu(float x, float alpha = 1.0f) {
        return x >= 0 ? x : alpha * (std::exp(x) - 1.0f);
    }
    static inline float relu(float x) {
        return std::max(0.0f, x);
    }

    static void board2tensor(const BoardState &board, float (&tensor)[768]);
};
