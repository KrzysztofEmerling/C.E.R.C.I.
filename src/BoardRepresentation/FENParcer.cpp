#include "FENParcer.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

Flags ParseFEN(const String fen, u64f (&bitboards)[13])
{
    Flags flags = {};
    flags.FlagsValue = 0ULL;

    for (int i = 0; i < 13; i++)
    {
        bitboards[i] = 0ULL;
    }

    int index = 0;
    int file = 0, rank = 7;

    while (fen[index] != ' ')
    {
        char piece = fen[index];

        if (piece >= '1' && piece <= '8')
        {
            file += piece - '0';
        }
        else if (piece == '/')
        {
            rank--;
            file = 0;
        }
        else
        {
            int boardIndex;
            switch (piece)
            {
            case 'p':
                boardIndex = BlackPawns;
                break;
            case 'r':
                boardIndex = BlackRooks;
                break;
            case 'n':
                boardIndex = BlackKnights;
                break;
            case 'b':
                boardIndex = BlackBishops;
                break;
            case 'q':
                boardIndex = BlackQueens;
                break;
            case 'k':
                boardIndex = BlackKing;
                break;
            case 'P':
                boardIndex = WhitePawns;
                break;
            case 'R':
                boardIndex = WhiteRooks;
                break;
            case 'N':
                boardIndex = WhiteKnights;
                break;
            case 'B':
                boardIndex = WhiteBishops;
                break;
            case 'Q':
                boardIndex = WhiteQueens;
                break;
            case 'K':
                boardIndex = WhiteKing;
                break;
            default:
                continue;
            }
            bitboards[boardIndex] |= BitboardsIndecies[rank * 8 + file];
            file++;
        }
        index++;
    }

    index++;
    flags.whiteOnMove = (fen[index] == 'w') ? 1 : 0;
    index += 2;

    while (fen[index] != ' ')
    {
        switch (fen[index])
        {
        case '-':
            break;
        case 'K':
            flags.whiteShortCastelRights = 1;
            break;
        case 'Q':
            flags.whiteLongCastelRights = 1;
            break;
        case 'k':
            flags.blackShortCastelRights = 1;
            break;
        case 'q':
            flags.blackLongCastelRights = 1;
            break;
        }
        index++;
    }

    index++;
    if (fen[index] != '-')
    {
        int epFile = fen[index] - 'a';
        int epRank = fen[index + 1] - '1';
        bitboards[Empassants] |= BitboardsIndecies[epRank * 8 + epFile];
        index += 2;
    }
    else
    {
        index++;
    }

    while (fen[index] == ' ')
        index++;
    flags.halfmoveClock = static_cast<u_int8_t>(std::atoi(&fen[index]));

    while (fen[index] >= '0' && fen[index] <= '9')
        index++;
    while (fen[index] == ' ')
        index++;

    flags.moves = static_cast<u_int16_t>(std::atoi(&fen[index]));

    return flags;
}

String SaveFEN(const Flags flags, u64f (&bitboards)[13])
{
    std::ostringstream fen;

    // Piece placement
    for (int rank = 7; rank >= 0; --rank)
    {
        int empty = 0;
        for (int file = 0; file < 8; ++file)
        {
            int square = rank * 8 + file;
            bool found = false;
            for (int i = 0; i < 12; ++i)
            {
                if (bitboards[i] & BitboardsIndecies[square])
                {
                    if (empty > 0)
                    {
                        fen << empty;
                        empty = 0;
                    }
                    switch (i)
                    {
                    case WhitePawns:
                        fen << 'P';
                        break;
                    case WhiteKnights:
                        fen << 'N';
                        break;
                    case WhiteBishops:
                        fen << 'B';
                        break;
                    case WhiteRooks:
                        fen << 'R';
                        break;
                    case WhiteQueens:
                        fen << 'Q';
                        break;
                    case WhiteKing:
                        fen << 'K';
                        break;
                    case BlackPawns:
                        fen << 'p';
                        break;
                    case BlackKnights:
                        fen << 'n';
                        break;
                    case BlackBishops:
                        fen << 'b';
                        break;
                    case BlackRooks:
                        fen << 'r';
                        break;
                    case BlackQueens:
                        fen << 'q';
                        break;
                    case BlackKing:
                        fen << 'k';
                        break;
                    }
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                ++empty;
            }
        }
        if (empty > 0)
            fen << empty;
        if (rank > 0)
            fen << '/';
    }
    fen << ' ' << (flags.whiteOnMove ? 'w' : 'b');

    fen << ' ';
    bool anyCastling = false;
    if (flags.whiteShortCastelRights)
    {
        fen << 'K';
        anyCastling = true;
    }
    if (flags.whiteLongCastelRights)
    {
        fen << 'Q';
        anyCastling = true;
    }
    if (flags.blackShortCastelRights)
    {
        fen << 'k';
        anyCastling = true;
    }
    if (flags.blackLongCastelRights)
    {
        fen << 'q';
        anyCastling = true;
    }
    if (!anyCastling)
    {
        fen << '-';
    }

    fen << ' ';
    if (bitboards[Empassants])
    {
        int square = __builtin_ctzll(bitboards[Empassants]);
        char file = 'a' + (square % 8);
        char rank = '1' + (square / 8);
        fen << file << rank;
    }
    else
    {
        fen << '-';
    }

    fen << ' ' << static_cast<int>(flags.halfmoveClock);
    fen << ' ' << static_cast<int>(flags.moves);

    return fen.str();
}
