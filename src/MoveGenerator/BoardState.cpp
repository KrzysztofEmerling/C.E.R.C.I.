#include "FENParcer.h"
#include "BoardState.h"
#include "MoveGenerator.h"

#include <iostream>

BoardState::BoardState(String FENnotation)
{
    m_Flags = ParseFEN(FENnotation, m_Pieces);
}

BoardState::BoardState(Flags flags, u64f (&pieces)[13]) : m_Flags(flags)
{
    for (int i = 0; i < 13; i++)
    {
        m_Pieces[i] = pieces[i];
    }
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
            const char *piece = " ";

            for (int i = 0; i < 12; i++)
            {
                if (m_Pieces[i] & (1ULL << index))
                {
                    piece = m_UnicodePieces[i];
                    break;
                }
            }
            std::cout << " " << piece << " |";
        }
        std::cout << " " << (rank + 1);

        if (rank == 7)
            std::cout << "\t\tNazwa Silnika: C.E.R.C.I\n";
        else if (rank == 6)
            std::cout << "\t\tChess Engine with Reinforcement-based\n";
        else if (rank == 5)
            std::cout << "\n";
        else if (rank == 4)
            std::cout << "\n";
        else if (rank == 3)
            std::cout << "\t\tBiałe na ruchu: " << (IsWhiteMove() ? "TAK (♚)\n" : "NIE (♔)\n");
        else if (rank == 2)
            std::cout << "\t\tPrawa do roszady: "
                      << (CanWhiteShortCastel() ? "K" : "-")
                      << (CanWhiteLongCastel() ? "Q " : "- ")
                      << (CanBlackShortCastel() ? "k" : "-")
                      << (CanBlackLongCastel() ? "q\n" : "-\n");

        else if (rank == 1)
            std::cout << "\t\tPółruchy: " << static_cast<int>(GetHalfMoves()) << "\n";
        else if (rank == 0)
            std::cout << "\t\tNumer ruchu: " << GetMoves() << "\n";
        else
            std::cout << "\n";

        if (rank == 6)
            std::cout << "   +---+---+---+---+---+---+---+---+\t\tCognitive Identities\n";
        else
            std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }

    std::cout << "     a   b   c   d   e   f   g   h   " << std::endl;
}

void BoardState::MakeMove(Move move)
{
    u64 startingPos = BitboardsIndecies[move.startingSquere];
    u64 destPos = BitboardsIndecies[move.destSquere];

    int figToMove = -1;
    int figToCapture = -1;

    for (int i = 0; i < 12; i++)
    {
        if (startingPos & m_Pieces[i])
            figToMove = i;
        if (destPos & m_Pieces[i])
            figToCapture = i;
    }

    // Reset en passant square na początek ruchu
    m_Pieces[Empassants] = 0ULL;

    switch (move.flag)
    {
    case NormalMove:

        // przypadki roszady
        if (figToMove == WhiteKing)
        {
            m_Flags.whiteShortCastelRights = false;
            m_Flags.whiteLongCastelRights = false;
        }
        else if (figToMove == WhiteRooks)
        {
            if (startingPos & 0x80)
                m_Flags.whiteShortCastelRights = false;
            else if (startingPos & 0x1)
                m_Flags.whiteLongCastelRights = false;
        }

        else if (figToMove == BlackKing)
        {
            m_Flags.blackShortCastelRights = false;
            m_Flags.blackLongCastelRights = false;
        }
        else if (figToMove == BlackRooks)
        {
            if (startingPos & 0x8000000000000000)
                m_Flags.blackShortCastelRights = false;
            else if (startingPos & 0x100000000000000)
                m_Flags.blackLongCastelRights = false;
        }
        // Przesuń pionek/figurę
        m_Pieces[figToMove] &= ~startingPos;
        m_Pieces[figToMove] |= destPos;

        // Jeśli bicie, usuń zbitego
        if (figToCapture > -1)
        {
            m_Pieces[figToCapture] &= ~destPos;
            m_Flags.halfmoveClock = 0;
        }
        break;

    case DoublePush:
    {
        // Przesuń pionka
        m_Pieces[figToMove] &= ~startingPos;
        m_Pieces[figToMove] |= destPos;

        // Ustaw pole en passant
        int direction = (figToMove == WhitePawns) ? -8 : 8;
        int epSquare = move.destSquere + direction;
        m_Pieces[Empassants] = BitboardsIndecies[epSquare];

        m_Flags.halfmoveClock = 0;
        break;
    }

    case PromotionQueen:
    case PromotionRook:
    case PromotionKnight:
    case PromotionBishop:
    {
        m_Flags.halfmoveClock = 0;
        int new_piece = -1;
        switch (move.flag)
        {
        case PromotionQueen:
            new_piece = (figToMove < 6) ? WhiteQueens : BlackQueens;
            break;
        case PromotionRook:
            new_piece = (figToMove < 6) ? WhiteRooks : BlackRooks;
            break;
        case PromotionKnight:
            new_piece = (figToMove < 6) ? WhiteKnights : BlackKnights;
            break;
        case PromotionBishop:
            new_piece = (figToMove < 6) ? WhiteBishops : BlackBishops;
            break;
        }

        // Usuń pionka z pola startowego
        m_Pieces[figToMove] &= ~startingPos;

        // Jeśli bicie, usuń zbitego
        if (figToCapture > -1)
        {
            m_Pieces[figToCapture] &= ~destPos;
        }

        // Dodaj nową figurę na polu docelowym
        m_Pieces[new_piece] |= destPos;
        break;
    }

    case EmpassantMove:
    {
        // Usuń pionka z pola startowego
        m_Pieces[figToMove] &= ~startingPos;
        // Dodaj pionka na polu docelowym
        m_Pieces[figToMove] |= destPos;

        // Usuń zbitego pionka en passant
        int epSquare = (figToMove == WhitePawns) ? move.destSquere - 8 : move.destSquere + 8;
        int capturedPawn = (figToMove == WhitePawns) ? BlackPawns : WhitePawns;
        m_Pieces[capturedPawn] &= ~BitboardsIndecies[epSquare];

        m_Flags.halfmoveClock = 0;
        break;
    }

    case Castling:
        if (m_Flags.whiteOnMove)
        {
            m_Flags.whiteShortCastelRights = false;
            m_Flags.whiteLongCastelRights = false;

            if (move.destSquere == 6)
            {
                m_Pieces[WhiteKing] = 0x40;

                m_Pieces[WhiteRooks] &= 0xffffffffffffff7f;
                m_Pieces[WhiteRooks] |= 0x20;
            }
            else
            {
                m_Pieces[WhiteKing] = 0x4;

                m_Pieces[WhiteRooks] &= 0xfffffffffffffffe;
                m_Pieces[WhiteRooks] |= 0x8;
            }
        }
        else
        {
            m_Flags.blackShortCastelRights = false;
            m_Flags.blackLongCastelRights = false;

            if (move.destSquere == 62)
            {
                m_Pieces[BlackKing] = 0x4000000000000000;

                m_Pieces[BlackRooks] &= 0x7fffffffffffffff;
                m_Pieces[BlackRooks] |= 0x2000000000000000;
            }
            else
            {
                m_Pieces[BlackKing] = 0x400000000000000;

                m_Pieces[BlackRooks] &= 0xfeffffffffffffff;
                m_Pieces[BlackRooks] |= 0x800000000000000;
            }
        }
        break;
    }

    // sprawdzenie zbicia wierzy w przypadku praw roszad
    if (figToCapture == WhiteRooks)
    {
        if (destPos & 0x80)
            m_Flags.whiteShortCastelRights = false;
        else if (destPos & 0x1)
            m_Flags.whiteLongCastelRights = false;
    }
    else if (figToCapture == BlackRooks)
    {
        if (destPos & 0x8000000000000000)
            m_Flags.blackShortCastelRights = false;
        else if (destPos & 0x100000000000000)
            m_Flags.blackLongCastelRights = false;
    }

    // Aktualizacja ruchu pełnego i zmiana strony
    if (!m_Flags.whiteOnMove)
        m_Flags.moves += 1;
    if (figToMove != WhitePawns && figToCapture == -1)
        m_Flags.halfmoveClock += 1;

    else if (figToMove != BlackPawns && figToCapture == -1)
        m_Flags.halfmoveClock += 1;

    m_Flags.whiteOnMove = !m_Flags.whiteOnMove;
}

bool BoardState::MakeMove(const char *move_notation)
{
    // Parsowanie z pełną obsługą promocji i flag
    int from = SquareIndex[move_notation[0] - 'a'][move_notation[1] - '1'];
    int to = SquareIndex[move_notation[2] - 'a'][move_notation[3] - '1'];

    // Walidacja
    std::queue<Move> moves;
    BoardState board(m_Flags, m_Pieces);
    MoveGenerator::GetLegalMoves(board, moves);

    while (!moves.empty())
    {
        Move move = moves.front();
        moves.pop();

        if (move.startingSquere == from && move.destSquere == to)
        {
            MakeMove(move);
            return true;
        }
    }
    return false;
}