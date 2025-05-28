#include "zHashCalculationTests.h"
#include "../PerftTestCase.h"
#include "../TestUtils.h"
#include <stdexcept>

u64 zHashCalculationTests::PerftWithZHashCheck(BoardState &state, int depth)
{
    if (state.GetHash() != state.GetRefHash())
    {
        throw std::logic_error("Mismatch zHash at fen:\n" + state.FEN());
    }

    if (depth == 0)
        return 1;

    u64 nodes = 0;
    u64 hashBefore = state.GetHash();

    MoveList moves;
    MoveGenerator::GetLegalMoves(state, moves);

    for (size_t i = 0; i < moves.movesCount; i++)
    {
        state.MakeMove(moves.moves[i]);
        nodes += PerftWithZHashCheck(state, depth - 1);
        state.UndoMove();
    }

    return nodes;
}

void zHashCalculationTests::RunTests()
{
    std::cout << "-------------------------------------------------------------\n";
    std::cout << "[zHash Calculation Test:]\n\n";

    auto testModuleTimeStart = high_resolution_clock::now();
    for (const auto &testCase : longTestCases)
    {

        std::cout << "Testing FEN: " << testCase.fen << "\n";
        BoardState state = BoardState(testCase.fen);
        try
        {
            auto testTimeStart = high_resolution_clock::now();

            PerftWithZHashCheck(state, 6);
            auto testTimeStop = high_resolution_clock::now();

            std::cout << "Test PASSED in (" << FormatTime(testTimeStop - testTimeStart) << ")\n"
                      << std::endl;
        }
        catch (const std::exception &e)
        {

            std::cout << e.what() << "\n";
            std::cout << "Test FAILED\n"
                      << std::endl;
        }
    }
    auto testModuleTimeStop = high_resolution_clock::now();

    auto testModuleTimeEnd = high_resolution_clock::now();
    std::cout << "Total zHash test time: " << FormatTime(testModuleTimeEnd - testModuleTimeStart) << "\n";
    std::cout << "-------------------------------------------------------------\n"
              << std::endl;
}