#pragma once
#include "BitboardsUtils.h"
#include "MoveGenerator.h"


constexpr uint8_t SquareIndeciesFromNotation(const char file, const char rank) {
    return (rank - '1') * 8 + (file - 'a');
}
constexpr Move ParseMove(const char* notation) {
    return Move{
        SquareIndeciesFromNotation(notation[0], notation[1]),
        SquareIndeciesFromNotation(notation[2], notation[3])
    };
}


class BoardState
{
private:
    
    Flags m_Flags;
    u64f m_Pieces[13];

    const char* UnicodePieces[13] = {
        "♟", "♞", "♝", "♜", "♛", "♚",  // Białe
        "♙", "♘", "♗", "♖", "♕", "♔",  // Czarne
        ""                    
    };
public:
    BoardState(String FENnotation);

    void DrawBoard() const;

    inline u64f GetBB(BitBoardsIndecis pieceType) const noexcept { return m_Pieces[pieceType]; }
    inline const u64f* GetBBs() const noexcept { return m_Pieces; }

    inline Flags GetFlags() const noexcept {return m_Flags; }
    inline bool IsWhiteMove() const noexcept {return m_Flags.uWhiteOnMove; }
    inline bool CanWhiteShortCastel() const noexcept {return m_Flags.uWhiteShortCastelRights; }
    inline bool CanWhiteLongCastel() const noexcept {return m_Flags.uWhiteLongCastelRights; }
    inline bool CanBlackShortCastel() const noexcept {return m_Flags.uBlackShortCastelRights; }
    inline bool CanBlackLongCastel() const noexcept {return m_Flags.uBlackLongCastelRights; }
    
    inline uint8_t GetHalfMoves() const noexcept {return m_Flags.uHalfmoveClock; }
    inline uint16_t GetMoves() const noexcept {return m_Flags.uMoves; }


    //unchecked
    void MakeMove(Move move);
    void MakeMove(const char* move_notation);

};