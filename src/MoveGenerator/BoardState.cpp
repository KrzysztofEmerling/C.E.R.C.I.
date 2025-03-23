#include "BoardState.h"
#include "FENParcer.h"

#include <iostream>

BoardState::BoardState(String FENnotation)
{
    m_Flags = ParseFEN(FENnotation, m_Pieces);
}

void BoardState::DrawBoard() const
{

    std::cout << "     a   b   c   d   e   f   g   h\n";
    std::cout << "   +---+---+---+---+---+---+---+---+\n";

    for (int rank = 7; rank >= 0; rank--) 
    {
        std::cout << " " << (rank + 1) << " |";
        for (int file = 0; file < 8; file++) 
        {
            int index = rank * 8 + file;
            const char* piece = " ";

            for (int i = 0; i < 12; i++) 
            {
                if (m_Pieces[i] & (1ULL << index)) 
                {
                    piece = UnicodePieces[i];
                    break;
                }
            }
            std::cout << " " << piece << " |";
        }
        std::cout << " " << (rank + 1);

        if (rank == 7) std::cout << "\t\tNazwa Silnika: C.E.R.C.I\n";
        else if (rank == 6) std::cout << "\t\tChess Engine with Reinforcement-based\n";
        else if (rank == 5) std::cout << "\n";
        else if (rank == 4) std::cout << "\n";
        else if (rank == 3) std::cout << "\t\tBiałe na ruchu: " << (IsWhiteMove() ? "TAK (♚)\n" : "NIE (♔)\n");
        else if (rank == 2) std::cout << "\t\tPrawa do roszady: "
                      << (CanWhiteShortCastel() ? "K" : "-")
                      << (CanWhiteLongCastel() ? "Q " : "- ")
                      << (CanBlackShortCastel() ? "k" : "-")
                      << (CanBlackLongCastel() ? "q\n": "-\n");

        else if (rank == 1) std::cout << "\t\tPółruchy: " << static_cast<int>(GetHalfMoves()) << "\n";
        else if (rank == 0) std::cout << "\t\tNumer ruchu: " << GetMoves() << "\n";
        else std::cout << "\n";
        
        if (rank == 6)std::cout << "   +---+---+---+---+---+---+---+---+\t\tCognitive Identities\n";
        else std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }

    std::cout << "     a   b   c   d   e   f   g   h   " << std::endl;
}

void BoardState::MakeMove(Move move)
{
    u64 starting_pos = BitboardsIndecies[move.uStartingSquere];
    u64 dest_pos = BitboardsIndecies[move.uDestSquere];

    int fig_to_move = -1;
    int fig_to_capture = -1;
    for(int i = 0; i < 12; i++)
    {
        if(starting_pos & m_Pieces[i])
        {
            fig_to_move = i;
        }
        else if(dest_pos & m_Pieces[i])
        {
            fig_to_capture = i;
        }
    }

    m_Pieces[fig_to_move] ^= dest_pos;

    m_Pieces[fig_to_move] ^= starting_pos;

    if(fig_to_capture > -1)
    {
        //m_Pieces[fig_to_capture] ^= dest_pos; w przypadku enpassanta doda pionka przeciwnikowi
        m_Pieces[fig_to_capture] &= ~dest_pos;
        m_Flags.uHalfmoveClock = 0;
    }
    else if(fig_to_move == WhitePawns || fig_to_move == BlackPawns) m_Flags.uHalfmoveClock = 0;
    else m_Flags.uHalfmoveClock += 1;

    if(!IsWhiteMove())m_Flags.uMoves += 1;  
    m_Flags.uWhiteOnMove = !m_Flags.uWhiteOnMove;
}

void BoardState::MakeMove(const char* move_notation)
{
    MakeMove(ParseMove(move_notation));
}