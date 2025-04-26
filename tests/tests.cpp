#include <iostream>
#include <queue>
#include <vector>
#include <chrono>

#include "../src/MoveGenerator/BoardState.h"
#include "../src/MoveGenerator/MoveGenerator.h"

using namespace std;
using namespace std::chrono;

// Struktura do trzymania wyników
struct TestResult
{
    std::string fen;
    int depth;
    uint64_t nodes; // ile ruchów w sumie
    double elapsed_ms;
};

// Funkcja pomocnicza do "perft" (przeglądanie ruchów na głębokość)
uint64_t Perft(BoardState &state, int depth)
{
    if (depth == 0)
        return 1;

    std::queue<Move> moves;
    MoveGenerator::GetLegalMoves(state, moves);

    uint64_t nodes = 0;

    while (!moves.empty())
    {
        Move move = moves.front();
        moves.pop();

        BoardState newState = state;
        newState.MakeMove(move);

        nodes += Perft(newState, depth - 1);
    }

    return nodes;
}

int main()
{
    // Twoje przykładowe pozycje
    vector<string> testFENs = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",         // startowa
        "r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 2 4" // włoska
    };

    vector<int> depths = {1, 2, 3}; // głębokości testowe
    vector<TestResult> results;

    for (const auto &fen : testFENs)
    {
        for (int depth : depths)
        {
            BoardState state(fen);

            auto start = high_resolution_clock::now();
            uint64_t nodes = Perft(state, depth);
            auto end = high_resolution_clock::now();

            double elapsed = duration<double, std::milli>(end - start).count();

            results.push_back({fen, depth, nodes, elapsed});
        }
    }

    // Wypisz wyniki
    for (const auto &result : results)
    {
        cout << "FEN: " << result.fen << endl;
        cout << "Depth: " << result.depth << "  Nodes: " << result.nodes << "  Time: " << result.elapsed_ms << " ms" << endl;
        cout << "-------------------------------------------------------------" << endl;
    }

    return 0;
}
