#include "DNN.h"

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

float DNN::W0[768 * 1024];
float DNN::b0[1024];
float DNN::W1[1024 * 512];
float DNN::b1[512];
float DNN::W2[512 * 1];
float DNN::b2[1];

void DNN::LoadWeights(const std::string& filepath)
{
    std::ifstream f(filepath, std::ios::binary);
    if (!f) {
        std::cerr << "Nie mogę otworzyć pliku!\n";
        std::cerr << "Próbowana ścieżka: " 
                << std::filesystem::absolute(filepath) << "\n";

        throw std::runtime_error("Nie mogę otworzyć pliku!");
    }


    auto read = [&](float* dst, size_t count) {
        f.read(reinterpret_cast<char*>(dst), count * sizeof(float));
        if (!f) throw std::runtime_error("Błąd czytania wag!");
    };

    read(W0, 768 * 1024);
    read(b0, 1024);
    read(W1, 1024 * 512);
    read(b1, 512);
    read(W2, 512 * 1);
    read(b2, 1);

    f.close();
}

float DNN::FitForward(const BoardState &board)
{
    float input[768];
    board2tensor(board, input);

    // Layer 0
    static float h0[1024];
    for (int j = 0; j < 1024; j++) {
        float sum = b0[j];
        for (int i = 0; i < 768; i++)
            sum += input[i] * W0[i * 1024 + j];
        h0[j] = std::tanh(sum);
    }

    // Layer 1
    static float h1[512];
    for (int j = 0; j < 512; j++) {
        float sum = b1[j];
        for (int i = 0; i < 1024; i++)
            sum += h0[i] * W1[i * 512 + j];
        h1[j] = elu(sum);
    }

    // Layer 2 — Dense(1), activation = linear
    float out = b2[0];
    for (int i = 0; i < 512; i++)
        out += h1[i] * W2[i * 1 + 0];

    return out;
}
