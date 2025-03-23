#pragma once
#include<queue>

#include "BitboardsUtils.h"
#include "BoardState.h"

struct Move
{
    uint8_t uStartingSquere;
    uint8_t uDestSquere;
};

class MoveGenerator
{
public:
    static void GetLegalMovesQueue(const BoardState &state, std::queue<Move> &moves);
private:
    static u64 GetPseudoLegaWhitePawnsMoves(const u64f powns, const u64 empty, const u64 black_with_enpassants);
    static u64 GetPseudoLegaBlackPawnsMoves(const u64f pawns, const u64 empty, const u64 white_with_enpassants);
    
    static u64 GetPseudoLegaKnightsMove(const u64f powns, const u64 allay);


    static inline constexpr u64 NOT_RANKS_1_2 = 0xFFFFFFFFFFFF0000;
    static inline constexpr u64 NOT_RANK_1 =    0xFFFFFFFFFFFFFF00;
    static inline constexpr u64 NOT_RANKS_7_8 = 0x0000FFFFFFFFFFFF;
    static inline constexpr u64 NOT_RANK_8 =    0x00FFFFFFFFFFFFFF;

    static inline constexpr u64 NOT_COL_H =     0x7F7F7F7F7F7F7F7F;
    static inline constexpr u64 NOT_COL_A =     0xFEFEFEFEFEFEFEFE;
    static inline constexpr u64 NOT_COLS_AB =   0xFCFCFCFCFCFCFCFC;
    static inline constexpr u64 NOT_COLS_GH =   0x3F3F3F3F3F3F3F3F;
};