#pragma once

#include <string>
#include "BoardState.h"

class Eval
{
public:
    // Ocena statyczna pozycji
    static int staticEval(const BoardState &board);

    // Algorytm alfa-beta z przycinaniem
    static int alphaBeta(BoardState &board, int depth, int alpha, int beta, bool maximizingPlayer);

    // Znajdź najlepszy ruch wg oceny i alfa-beta
    static std::string findBestMove(BoardState &board, int depth);

private:
    // Piece-square tables dla początku i końca gry (tablice 64 elementowe)
    static const int pawnTableOpening[64];
    static const int knightTableOpening[64];
    static const int bishopTableOpening[64];
    static const int rookTableOpening[64];
    static const int queenTableOpening[64];
    static const int kingTableOpeningOpening[64];

    static const int pawnTableEndgame[64];
    static const int knightTableEndgame[64];
    static const int bishopTableEndgame[64];
    static const int rookTableEndgame[64];
    static const int queenTableEndgame[64];
    static const int kingTableEndgame[64];

    // Pomocnicza funkcja do pobrania wartości z tabeli aktywności
    // static int getPieceSquareValue(int square, PieceType type, Color color, bool endgame);

    // Prosta heurystyka określająca czy jest końcówka gry
    // static bool isEndgame(const BoardState &board);
};