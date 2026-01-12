#include "DNN.h"
#include "agressiveMCTSv5.1Weights.h"

#include <immintrin.h>
#include <cmath>
#include <algorithm>

#include <cstring>      
#include <string>       
#include <stdexcept>    
#include <fstream> 

#include <filesystem>
#include <iostream>
void DNN::board2tensor(const BoardState &board, float (&tensor)[768])
{
    const u64f *pieces = board.GetBBs();
    std::memset(tensor, 0, sizeof(tensor));

    for (int i = 0; i < 12; i++)
    {
        int offset = i * 64;
        u64f piece = pieces[i];
        while (piece)
        {
            int square = __builtin_ctzll(piece);
            piece &= piece - 1;

            tensor[offset + square] = 1.0f;
        }
    }
}

float DNN::FitForward(const BoardState &board)
{
    float input[768];
    board2tensor(board, input);

    // Layer 0 - Dense(1024), ELU
    float h0[1024];
    for (int j = 0; j < 1024; j++) {
        float sum = layer0b[j];
        for (int i = 0; i < 768; i++)
            sum += input[i] * layer0w[i][j];
        h0[j] = elu(sum);
    }

    // Layer 1- Dense(1024), tanh
    float h1[1024];
    for (int j = 0; j < 1024; j++) {
        float sum = layer1b[j];
        for (int i = 0; i < 1024; i++)
            sum += h0[i] * layer1w[i][j];
        h1[j] = std::tanh(sum);
    }

    // Layer 2 - Dense(16), ELU
    float h2[16];
    for (int j = 0; j < 16; j++) {
        float sum = layer2b[j];
        for (int i = 0; i < 1024; i++)
            sum += h1[i] * layer2w[i][j];
        h2[j] = elu(sum);
    }

    // Layer 3 -Dense(1), linear
    float out = layer3b[0];
    for (int i = 0; i < 16; i++)
        out += h2[i] * layer3w[i][0];

    return out;
}
