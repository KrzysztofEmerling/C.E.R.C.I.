#pragma once

#include "BoardState.h"
#include "MoveGenerator.h"

u64 Perft(BoardState &state, int depth);

u64 PerftCount(BoardState &state, int depth);
