#include "zHashCalculationTests.h"
#include "../PerftTestCase.h"
u64 zHashCalculationTests::PerftWithZHashCheck(BoardState &state, int depth)
{
    if (depth == 0)
        return 1;

    u64 nodes = 0;
    u64 hashBefore = state.GetHash();

    MoveList moves;
    MoveGenerator::GetLegalMoves(state, moves);

    // TODO: dokończenie logiki perft testów dla zHash

    return nodes;
}

void zHashCalculationTests::RunLongTests()
{
    std::cout << "-------------------------------------------------------------\n";
    std::cout << "[Long MoveGenerator Test:]\n\n";

    auto testModuleTimeStart = high_resolution_clock::now();
    for (const auto &testCase : longTestCases)
    {
        std::cout << "Testing FEN: " << testCase.fen << "\n";

        for (const auto &[depth, expectedNodes] : testCase.expectedNodes)
        {
            BoardState state(testCase.fen);

            auto start = high_resolution_clock::now();
            u64 nodes = PerftCount(state, depth);
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
                Perft(state, depth);
                std::cout << "-------------------------------------------------------------\n";
                return;
            }
        }
        std::cout << "-------------------------------------------------------------\n";
    }
    auto testModuleTimeEnd = high_resolution_clock::now();
    std::cout << "Total long preft test time: " << testModuleTimeEnd - testModuleTimeStart << "\n";
    std::cout << "-------------------------------------------------------------\n"
              << std::endl;
}

void zHashCalculationTests::RunQuickTests()
{

    std::cout << "-------------------------------------------------------------\n";
    std::cout << "[Quick MoveGenerator Test:]\n\n";
    auto testModuleTimeStart = high_resolution_clock::now();

    for (const auto &testCase : quickTestCases)
    {
        const auto &[depth, expectedNodes] = *testCase.expectedNodes.find(5);
        BoardState state(testCase.fen);
        auto start = high_resolution_clock::now();
        u64 nodes = PerftCount(state, depth);
        auto end = high_resolution_clock::now();

        double elapsed = duration<double, std::milli>(end - start).count();
        bool passed = (nodes == expectedNodes);

        std::cout << "FEN: " << testCase.fen << "\n";
        std::cout << "Depth " << depth
                  << ": Nodes = " << nodes
                  << ", Expected = " << expectedNodes
                  << ", " << (passed ? "PASSED" : "FAILED")
                  << " (" << elapsed << " ms)\n\n";
    }
    auto testModuleTimeEnd = high_resolution_clock::now();
    std::cout << "-------------------------------------------------------------\n";
    std::cout << "Total quick perft test time: " << testModuleTimeEnd - testModuleTimeStart << "\n";
    std::cout << "-------------------------------------------------------------\n";
}