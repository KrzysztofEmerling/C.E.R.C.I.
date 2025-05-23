#include "PerftTests.h"

void PerftTests::RunLongTests()
{
    std::vector<PerftTestCase> testCases = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // Start position
         {
             {1, 20},
             {2, 400},
             {3, 8902},
             {4, 197281},
             {5, 4865609},
             {6, 119060324}}},

        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", // Kiwipete
         {
             {1, 48},
             {2, 2039},
             {3, 97862},
             {4, 4085603},
             {5, 193690690},
             {6, 8031647685}}},

        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", // Bug hunter
         {
             {1, 44},
             {2, 1486},
             {3, 62379},
             {4, 2103487},
             {5, 89941194}}},

        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", // Node crunch
         {
             {1, 46},
             {2, 2079},
             {3, 89890},
             {4, 3894594},
             {5, 164075551},
             {6, 6923051137}}},

        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", // position 4
         {
             {1, 6},
             {2, 264},
             {3, 9467},
             {4, 422333},
             {5, 15833292},
             {6, 706045033}}},

        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", // it always was enpassant...
         {
             {1, 14},
             {2, 191},
             {3, 2812},
             {4, 43238},
             {5, 674624},
             {6, 11030083}}}};

    auto testModuleTimeStart = high_resolution_clock::now();
    for (const auto &testCase : testCases)
    {
        std::cout << "Testing FEN: " << testCase.fen << "\n";

        for (const auto &[depth, expectedNodes] : testCase.expectedNodes)
        {
            BoardState state(testCase.fen);

            auto start = high_resolution_clock::now();
            uint64_t nodes = PerftCount(state, depth);
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
    std::cout << "-------------------------------------------------------------" << std::endl;
}

void PerftTests::RunQuickTest()
{
    std::vector<PerftTestCase> testCases = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", {{5, 4865609}}},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", {{5, 193690690}}},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", {{5, 89941194}}},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", {{5, 164075551}}},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", {{5, 15833292}}},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", {{5, 674624}}},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", {{5, 89941194}}}};

    std::cout << "-------------------------------------------------------------\n";
    std::cout << "[Quick MoveGenerator Test:]\n\n";
    auto testModuleTimeStart = high_resolution_clock::now();

    for (const auto &testCase : testCases)
    {
        const auto &[depth, expectedNodes] = *testCase.expectedNodes.find(5);
        BoardState state(testCase.fen);
        auto start = high_resolution_clock::now();
        uint64_t nodes = PerftCount(state, depth);
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
    std::cout << "Total quick perft test time: " << testModuleTimeEnd - testModuleTimeStart << "\n";
    std::cout << "-------------------------------------------------------------\n";
}