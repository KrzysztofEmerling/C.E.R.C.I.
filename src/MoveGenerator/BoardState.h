#pragma once
#include "BitboardsUtils.h"

constexpr uint8_t SquareIndex[8][8] = {
    {0, 8, 16, 24, 32, 40, 48, 56},  // a1, a2, a3, ..., a8
    {1, 9, 17, 25, 33, 41, 49, 57},  // b1, b2, b3, ..., b8
    {2, 10, 18, 26, 34, 42, 50, 58}, // c1, c2, c3, ..., c8
    {3, 11, 19, 27, 35, 43, 51, 59}, // d1, d2, d3, ..., d8
    {4, 12, 20, 28, 36, 44, 52, 60}, // e1, e2, e3, ..., e8
    {5, 13, 21, 29, 37, 45, 53, 61}, // f1, f2, f3, ..., f8
    {6, 14, 22, 30, 38, 46, 54, 62}, // g1, g2, g3, ..., g8
    {7, 15, 23, 31, 39, 47, 55, 63}  // h1, h2, h3, ..., h8
};

class BoardState
{
private:
    Flags m_Flags;
    u64f m_Pieces[13];

    const char *m_UnicodePieces[13] = {
        "♟", "♞", "♝", "♜", "♛", "♚", // Białe
        "♙", "♘", "♗", "♖", "♕", "♔", // Czarne
        "."};

public:
    BoardState(String FENnotation);

    void DrawBoard() const;

    inline u64f GetBB(BitBoardsIndecis pieceType) const noexcept { return m_Pieces[pieceType]; }
    inline const u64f *GetBBs() const noexcept { return m_Pieces; }

    inline Flags GetFlags() const noexcept { return m_Flags; }
    inline bool IsWhiteMove() const noexcept { return m_Flags.whiteOnMove; }
    inline bool CanWhiteShortCastel() const noexcept { return m_Flags.whiteShortCastelRights; }
    inline bool CanWhiteLongCastel() const noexcept { return m_Flags.whiteLongCastelRights; }
    inline bool CanBlackShortCastel() const noexcept { return m_Flags.blackShortCastelRights; }
    inline bool CanBlackLongCastel() const noexcept { return m_Flags.blackLongCastelRights; }

    inline uint8_t GetHalfMoves() const noexcept { return m_Flags.halfmoveClock; }
    inline uint16_t GetMoves() const noexcept { return m_Flags.moves; }

    // unchecked
    void MakeMove(Move move);
    void MakeMove(const char *move_notation);
};