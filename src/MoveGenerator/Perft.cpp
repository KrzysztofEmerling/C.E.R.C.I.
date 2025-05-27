#include "Perft.h"

#include <iostream>
#include <queue>

u64 PerftCount(BoardState &state, int depth)
{
    if (depth == 0)
        return 1;

    MoveList movesList;
    MoveGenerator::GetLegalMoves(state, movesList);
    u64 nodes = 0;

    for (int i = 0; i < movesList.movesCount; i++)
    {
        state.MakeMove(movesList.moves[i]);
        nodes += PerftCount(state, depth - 1);
        state.UndoMove();
    }

    return nodes;
}

u64 Perft(BoardState &state, int depth)
{
    if (depth == 0)
        return 1;

    MoveList movesList;
    MoveGenerator::GetLegalMoves(state, movesList);
    u64 totalNodes = 0ULL;
    u64 nodes = 0ULL;

    for (int i = 0; i < movesList.movesCount; i++)
    {
        Move move = movesList.moves[i];

        state.MakeMove(movesList.moves[i]);
        nodes += PerftCount(state, depth - 1);
        state.UndoMove();

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
