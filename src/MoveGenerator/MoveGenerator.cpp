#include "MoveGenerator.h"
void MoveGenerator::GetLegalMovesQueue(const BoardState &state, std::queue<Move> &moves)
{
    const u64f* pieces = state.GetBBs();
    const Flags flags = state.GetFlags();

    u64 white = pieces[0] | pieces[1] | pieces[2]|
                pieces[3] | pieces[4] | pieces[5];

    u64 black = pieces[6] | pieces[7] | pieces[8]|
                pieces[9] | pieces[10] | pieces[11];

    u64 empty = ~(white | black);


    u64 pl_pawns;
    u64 pl_knights;
    if(flags.uWhiteOnMove)
    {
        pl_pawns = GetPseudoLegaWhitePawnsMoves(pieces[WhitePawns], empty, black | pieces[Empassants]);
        pl_knights = GetPseudoLegaKnightsMove(pieces[WhiteKnights], white);
    }
    else
    {
        pl_pawns = GetPseudoLegaBlackPawnsMoves(pieces[BlackPawns], empty, white | pieces[Empassants]);
        pl_knights = GetPseudoLegaKnightsMove(pieces[BlackKnights], black);

    }
}

u64 MoveGenerator::GetPseudoLegaWhitePawnsMoves(const u64f pawns, const u64 empty, const u64 black_with_enpassants)
{
    u64 singlePush = (pawns << 8) & empty;
    u64 doublePush = ((singlePush & 0x0000000000FF0000) << 8) & empty;

                                            //uwzględnienie granic planszy
    u64 leftAttack = (pawns << 7) & black_with_enpassants & NOT_COL_A; 
    u64 rightAttack = (pawns << 9) & black_with_enpassants & NOT_COL_H;

    return singlePush | doublePush | leftAttack | rightAttack;
}
u64 MoveGenerator::GetPseudoLegaBlackPawnsMoves(const u64f pawns, const u64 empty, const u64 white_with_enpassants)
{
    u64 singlePush = (pawns << 8) & empty;
    u64 doublePush = ((singlePush & 0x0000FF0000000000) << 8) & empty;

                                            //uwzględnienie granic planszy
    u64 leftAttack = (pawns << 7) & white_with_enpassants & NOT_COL_A; 
    u64 rightAttack = (pawns << 9) & white_with_enpassants & NOT_COL_H;

    return singlePush | doublePush | leftAttack | rightAttack;
}

u64 MoveGenerator::GetPseudoLegaKnightsMove(const u64f knights, const u64 allay)
{
            // 2 w górę 1 w prawo
    return  (((NOT_RANKS_1_2 & NOT_COL_H & knights) << 17) |
            // 2 w górę 1 w lewo
            ((NOT_RANKS_1_2 & NOT_COL_A & knights) << 15)  |
            // 1 w górę 2 w prawo
            ((NOT_RANK_1 & NOT_COLS_AB & knights) << 10)   |
            // 1 w gurę 2 w lewo
            ((NOT_RANK_1 & NOT_COLS_GH & knights) << 6)    |
            
            // 2 w dół 1 w prawo
            ((NOT_RANKS_7_8 & NOT_COL_H & knights) >> 17)  |
            // 2 w dół 1 w lewo
            ((NOT_RANKS_7_8 & NOT_COL_A & knights) >> 15)  |
            // 1 w dół 2 w prawo
            ((NOT_RANK_8 & NOT_COLS_AB & knights) >> 10)   |
            // 1 w dół 2 w lewo
            ((NOT_RANK_8 & NOT_COLS_GH & knights) >> 6))   & ~allay;
}

