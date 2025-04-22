#include "MoveGenerator.h"
#include "MagicBitboards/RookMagicBitboards.h"
#include "MagicBitboards/BishopMagicBitboards.h"

void MoveGenerator::GetLegalMovesBBs(const BoardState &state, u64f &LegalMovesBBs[6])
{
    for(int i = 0; i < 6; i++)    { LegalMovesBBs[i] = 0ULL; }

    const u64f* pieces = state.GetBBs();
    const Flags flags = state.GetFlags();

    u64 white = pieces[0] | pieces[1] | pieces[2]|
                pieces[3] | pieces[4] | pieces[5];

    u64 black = pieces[6] | pieces[7] | pieces[8]|
                pieces[9] | pieces[10] | pieces[11];

    u64 all = white | black;
    u64 empty = ~all;


    u64 plPawns, oponentPawnsAttacks;
    u64 plKnights, oponentKnightsAttacks;
    u64 plBishops, oponentBishopsAttacks;
    u64 plRooks, oponentRooksAttacks;
    u64 plQueens, oponentQueensAttacks;
    u64 plKing, oponentKingAttacks;

    if(flags.uWhiteOnMove)
    {
        u64 notAllay = ~white;
        u64 notOponent = ~black;
        u64 allWithoutKing = all & (~ pieces[WhiteKing]);

        //atakowane pola
        oponentPawnsAttacks = GetPseudoLegalBlackPawnsAttacks(pieces[BlackPawns]);
        oponentKnightsAttacks = GetPseudoLegalKnightsMoves(pieces[BlackKnights], notOponent);
        oponentBishopsAttacks = GetPseudoLegalBishopsMoves(pieces[BlackBishops], notOponent, allWithoutKing);
        oponentRooksAttacks = GetPseudoLegalRooksMoves(pieces[BlackRooks], notOponent, allWithoutKing);
        oponentQueensAttacks = GetPseudoLegalQueensMoves(pieces[BlackQueens], notOponent, allWithoutKing);
        oponentKingAttacks = GetPseudoLegalKingMoves(pieces[BlackKing], notOponent);
        u64 underAttack = plOponentKnights | plOponentBishops | plOponentRooks | plOponentQueens | plOponentKing;

        //legalne ruchy
        plKing = GetPseudoLegalKingMoves(pieces[WhiteKing], notAllay);
        u64 attackers = 0ULL;
        if(pieces[WhiteKing] & underAttack)
        {
            attackers |= GetKnightsAttacks(pieces[WhiteKing]) & pieces[BlackKnights];
            attackers |= GetPseudoLegalBishopMoves(pieces[WhiteKing],0xffffffffffffffff,  all) & (pieces[BlackBishops] | pieces[BlackQueens]);
            attackers |= GetPseudoLegalRooksMoves(pieces[WhiteKing],0xffffffffffffffff,  all) & (pieces[BlackRooks] | pieces[BlackQueens]);
            
            if(std::popcount(attackers) > 1)
            {
                //zwracam tylko legalne ruchy króla z pod podwójnego szachu
                LegalMovesBBs[WhiteKing] = plKing & underAttack;
                return;
            }
            //możliwe ruchy to zbicie szachującej bierki, albo w niekturych przypadkach przysłonięcie jej
            else
            {
                int kingSquare = __builtin_ctzll(pieces[WhiteKing]);
                int attackerSquare = __builtin_ctzll(attackers);
            
                u64 posibleMovesMask = GetBetweenSquares(kingSquare, attackerSquare) | attackers;
                u64 pinedPiecesMask = 

                //TODO: Sprawdzenie związań
                plPawns = GetPseudoLegalWhitePawnsMoves(pieces[WhitePawns], empty, black | pieces[Empassants]);
                plKnights = GetPseudoLegalKnightsMoves(pieces[WhiteKnights], notAllay);
                plBishops = GetPseudoLegalBishopsMoves(pieces[WhiteBishops], notAllay, all);
                plRooks = GetPseudoLegalRooksMoves(pieces[WhiteRooks], notAllay, all);
                plQueens = GetPseudoLegalQueensMoves(pieces[WhiteQueens], notAllay, all);

                LegalMovesBBs[WhitePawns] = plKing & posibleMovesMask;
                LegalMovesBBs[WhiteKnights] = plKnights & posibleMovesMask;
                LegalMovesBBs[WhiteBishops] = plBishops & posibleMovesMask;
                LegalMovesBBs[WhiteRooks] = plRooks & posibleMovesMask;
                LegalMovesBBs[WhiteQueens] = plQueens & posibleMovesMask;
                LegalMovesBBs[WhiteKing] = plKing & underAttack;
                return;
            }
        }
        //możliwe wszystkie ruchy trzeba sprawdzić i dodać roszady
        //odrzucić związane figury
        //dokonać sprawdzenia dla związanego enpassanta
        else
        {
            
        }
        plKing &= underAttack;

        //generowane gdy król nie jest szachowany

        

    }
    else
    {
        //to samo dla czarnych
    }
}
u64 MoveGenerator::GetPseudoLegalWhitePawnsMoves(const u64f pawns, const u64 empty, const u64 black_with_enpassants)
{
    u64 singlePush = (pawns << 8) & empty;
    u64 doublePush = ((singlePush & 0x0000000000FF0000) << 8) & empty;

                                            //uwzględnienie granic planszy
    u64 leftAttack = (pawns << 7) & black_with_enpassants & NOT_COL_A; 
    u64 rightAttack = (pawns << 9) & black_with_enpassants & NOT_COL_H;

    return singlePush | doublePush | leftAttack | rightAttack;
}
u64 MoveGenerator::GetPseudoLegalWhitePawnsAttacks(const u64f pawns)
{
    u64 leftAttack = (pawns << 7) & NOT_COL_A; 
    u64 rightAttack = (pawns << 9) & NOT_COL_H;


    return leftAttack | rightAttack;
}

u64 MoveGenerator::GetPseudoLegalBlackPawnsMoves(const u64f pawns, const u64 empty, const u64 white_with_enpassants)
{
    u64 singlePush = (pawns << 8) & empty;
    u64 doublePush = ((singlePush & 0x0000FF0000000000) << 8) & empty;

                                            //uwzględnienie granic planszy
    u64 leftAttack = (pawns << 7) & white_with_enpassants & NOT_COL_A; 
    u64 rightAttack = (pawns << 9) & white_with_enpassants & NOT_COL_H;

    return singlePush | doublePush | leftAttack | rightAttack;
}
u64 MoveGenerator::GetPseudoLegalBlackPawnsAttacks(const u64f pawns)
{
    u64 leftAttack = (pawns << 7) & NOT_COL_A; 
    u64 rightAttack = (pawns << 9) & NOT_COL_H;

    return leftAttack | rightAttack;
}


u64 MoveGenerator::GetPseudoLegalKnightsMoves(const u64f knights, const u64 notAllay)
{
            // 2 w górę 1 w prawo
    return  (((NOT_COL_H & knights) << 17) |
            // 2 w górę 1 w lewo
            ((NOT_COL_A & knights) << 15)  |
            // 1 w górę 2 w prawo
            ((NOT_COLS_AB & knights) << 10)|
            // 1 w gurę 2 w lewo
            ((NOT_COLS_GH & knights) << 6) |
            
            // 2 w dół 1 w prawo
            ((NOT_COL_H & knights) >> 17)  |
            // 2 w dół 1 w lewo
            ((NOT_COL_A & knights) >> 15)  |
            // 1 w dół 2 w prawo
            ((NOT_COLS_AB & knights) >> 10)|
            // 1 w dół 2 w lewo
            ((& NOT_COLS_GH & knights) >> 6))   & notAllay;
}
u64 MoveGenerator::GetKnightsAttacks(const u64f knights)
{
            // 2 w górę 1 w prawo
    return  (((NOT_COL_H & knights) << 17) |
            // 2 w górę 1 w lewo
            ((NOT_COL_A & knights) << 15)  |
            // 1 w górę 2 w prawo
            ((NOT_COLS_AB & knights) << 10)|
            // 1 w gurę 2 w lewo
            ((NOT_COLS_GH & knights) << 6) |
            
            // 2 w dół 1 w prawo
            ((NOT_COL_H & knights) >> 17)  |
            // 2 w dół 1 w lewo
            ((NOT_COL_A & knights) >> 15)  |
            // 1 w dół 2 w prawo
            ((NOT_COLS_AB & knights) >> 10)|
            // 1 w dół 2 w lewo
            ((& NOT_COLS_GH & knights) >> 6));
}

u64 MoveGenerator::GetPseudoLegalBishopsMoves(const u64f bishops, const u64 notAllay, const u64 blockers)
{
    u64 moves = 0ULL;
    u64 tempBishops = bishops;

    while (tempBishops)
    {
        int square = __builtin_ctzll(tempBishops); 
        tempBishops &= tempBishops - 1; 

        u64 blockersMask = BishopMagicBitboards::GetBlockersMask(square);
        u64 relevantBlockers = blockers & blockersMask;

        u64 bishopMoves = BishopMagicBitboards::GetMovesMask(square, relevantBlockers);
        moves |= bishopMoves & notAllay; 
    }
    return moves;
}

u64 MoveGenerator::GetPseudoLegalRooksMoves(const u64f rooks, const u64 notAllay, const u64 blockers)
{
    u64 moves = 0ULL;
    u64 tempRooks = rooks;

    while (tempRooks)
    {
        int square = __builtin_ctzll(tempRooks); 
        tempRooks &= tempRooks - 1; 

        u64 blockersMask = RookMagicBitboards::GetBlockersMask(square);
        u64 relevantBlockers = blockers & blockersMask;

        u64 rookMoves = RookMagicBitboards::GetMovesMask(square, relevantBlockers);
        moves |= rookMoves & notAllay; 
    }
    return moves;
}

u64 MoveGenerator::GetPseudoLegalQueensMoves(const u64f queens, const u64 notAllay, const u64 blockers)
{
    u64 moves = 0ULL;
    u64 tempQueens = queens;

    while (tempQueens)
    {
        int square = __builtin_ctzll(tempQueens); 
        tempQueens &= tempQueens - 1; 

        u64 rookBlockersMask = RookMagicBitboards::GetBlockersMask(square);
        u64 bishopBlockersMask = BishopMagicBitboards::GetBlockersMask(square);

        u64 queenMoves = RookMagicBitboards::GetMovesMask(square, blockers & rookBlockersMask);
        u64 queenMoves |= BishopMagicBitboards::GetMovesMask(square, blockers & bishopBlockersMask);
        moves |= queenMoves & notAllay; 
    }
    return moves;
}

u64 MoveGenerator::GetPseudoLegalKingMoves(const u64f king, const u64 notAllay)
{
    u64 moves = 0ULL;
    moves |= (king << 8);               // Up
    moves |= (king >> 8);               // Down

    moves |= (king << 1) & NOT_COL_A;   // Right
    moves |= (king >> 1) & NOT_COL_H;   // Left

    moves |= (king << 9) & NOT_COL_A;   // Up-Right
    moves |= (king << 7) & NOT_COL_H;   // Up-Left
    moves |= (king >> 7) & NOT_COL_A;   // Down-Right
    moves |= (king >> 9) & NOT_COL_H;   // Down-Left

    return moves & notAllay;
}

u64 MoveGenerator::GetPinsMask(const u64f king, const u64 oponent, const u64 allay)
{
    //znalezienie atakujących sliderów na przeciw króla zapisanie do bitboarda
    
    //dla wszystkich (gońców, wierz, hetmanów przeciwnika) po & z bitboardem 
        
        //sprawdzenie czy pomiędzy sliderem i królem jest dkoładnie 1 twoja bierka

        //jeśli tak dodanie pól przed królem i do bierki slidera włączne do PinsMask
}