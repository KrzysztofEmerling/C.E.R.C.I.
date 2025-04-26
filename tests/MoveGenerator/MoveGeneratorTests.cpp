#include <iostream>
#include <queue>
#include <vector>
#include <map> // zamiast unordered_map, żeby mieć gwarantowaną kolejność po depth
#include <chrono>

#include "../../src/MoveGenerator/BoardState.h"
#include "../../src/MoveGenerator/MoveGenerator.h"

using namespace std::chrono;

struct PerftTestCase
{
    std::string fen;
    std::map<int, u64> expectedNodes; // map zamiast unordered_map -> gwarantuje kolejność depth
};

class TestMoveGenerator
{
public:
    static void RunPerftTests()
    {
        std::vector<PerftTestCase> testCases = {
            {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // Start position
             {
                 {1, 20},
                 {2, 400},
                 {3, 8902},
                 {4, 197281},
                 {5, 4865609}}},
            {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", // Kiwipete
             {
                 {1, 48},
                 {2, 2039},
                 {3, 97862},
                 {4, 4085603},
                 {5, 193690690}}}};

        for (const auto &testCase : testCases)
        {
            std::cout << "Testing FEN: " << testCase.fen << "\n";

            for (const auto &[depth, expectedNodes] : testCase.expectedNodes)
            {
                BoardState state(testCase.fen);

                auto start = high_resolution_clock::now();
                uint64_t nodes = Perft(state, depth);
                auto end = high_resolution_clock::now();

                double elapsed = duration<double, std::milli>(end - start).count();

                bool passed = (nodes == expectedNodes);

                std::cout << "Depth " << depth
                          << ": Nodes = " << nodes
                          << ", Expected = " << expectedNodes
                          << ", " << (passed ? "PASSED" : "FAILED")
                          << " (" << elapsed << " ms)"
                          << "\n";

                if (!passed)
                {
                    PerftPrint(state, depth);
                    std::cout << "-------------------------------------------------------------" << std::endl;
                    return;
                }
            }

            std::cout << "-------------------------------------------------------------" << std::endl;
        }
    }

private:
    static u64 Perft(BoardState &state, int depth)
    {
        if (depth == 0)
            return 1;

        std::queue<Move> moves;
        MoveGenerator::GetLegalMoves(state, moves);

        u64 nodes = 0;

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

    static u64 PerftPrint(BoardState &state, int depth)
    {
        if (depth == 0)
            return 1;

        std::queue<Move> moves;
        MoveGenerator::GetLegalMoves(state, moves);

        u64 totalNodes = 0;

        // Dla każdego ruchu pierwszego poziomu zliczamy nodes i wypisujemy
        while (!moves.empty())
        {
            Move move = moves.front();
            moves.pop();

            BoardState newState = state;
            newState.MakeMove(move);

            u64 nodes = Perft(newState, depth - 1);

            // Zamiana indeksów na notację UCI
            char f1 = 'a' + (move.startingSquere % 8);
            char r1 = '1' + (move.startingSquere / 8);
            char f2 = 'a' + (move.destSquere % 8);
            char r2 = '1' + (move.destSquere / 8);

            std::cout << f1 << r1 << f2 << r2 << ": " << nodes << std::endl;

            totalNodes += nodes;
        }

        std::cout << "Nodes searched: " << totalNodes << std::endl;
        return totalNodes;
    }
};