#include "Debug.h"

#include <iostream>
#include <queue>

u64 Debug::PerftCount(BoardState &state, int depth)
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

        nodes += PerftCount(newState, depth - 1);
    }

    return nodes;
}

u64 Debug::Perft(BoardState &state, int depth)
{
    if (depth == 0)
        return 1;

    std::queue<Move> moves;
    MoveGenerator::GetLegalMoves(state, moves);

    u64 totalNodes = 0;

    // później usuń!
    // std::cout << state.GetBB(Empassants) << std::endl;

    // Dla każdego ruchu pierwszego poziomu zliczamy nodes i wypisujemy
    while (!moves.empty())
    {
        Move move = moves.front();
        moves.pop();

        BoardState newState = state;
        newState.MakeMove(move);

        u64 nodes = PerftCount(newState, depth - 1);

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
