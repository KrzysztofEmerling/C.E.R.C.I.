#include "FENParcer.h"
#include <cstdlib> 

#include <iostream>

Flags ParseFEN(const String fen, u64f (&bitboards)[13]) 
{
    Flags flags = {};
    flags.uFlagsValue = 0ULL;  

    for (int i = 0; i < 13; i++) {
        bitboards[i] = 0ULL;
    }

    int index = 0;
    int file = 0, rank = 7; 

    while (fen[index] != ' ') {
        char piece = fen[index];

        if (piece >= '1' && piece <= '8') {  
            file += piece - '0'; 
        } 
        else if (piece == '/') {  
            rank--;  
            file = 0;
        } 
        else {  
            int boardIndex;
            switch (piece) {
                case 'p': boardIndex = BlackPawns; break;
                case 'r': boardIndex = BlackRooks; break;
                case 'n': boardIndex = BlackKnights; break;
                case 'b': boardIndex = BlackBishops; break;
                case 'q': boardIndex = BlackQueens; break;
                case 'k': boardIndex = BlackKing; break;
                case 'P': boardIndex = WhitePawns; break;
                case 'R': boardIndex = WhiteRooks; break;
                case 'N': boardIndex = WhiteKnights; break;
                case 'B': boardIndex = WhiteBishops; break;
                case 'Q': boardIndex = WhiteQueens; break;
                case 'K': boardIndex = WhiteKing; break;
                default: continue;
            }
            bitboards[boardIndex] |= BitboardsIndecies[rank * 8 + file];
            file++;
        }
        index++;
    }

    index++;  
    flags.uWhiteOnMove = (fen[index] == 'w') ? 1 : 0;
    index += 2;

    while (fen[index] != ' ') {
        switch (fen[index]) {
            case '-': break;
            case 'K': flags.uWhiteShortCastelRights = 1; break;
            case 'Q': flags.uWhiteLongCastelRights = 1; break;
            case 'k': flags.uBlackShortCastelRights = 1; break;
            case 'q': flags.uBlackLongCastelRights = 1; break;
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

    while (fen[index] == ' ') index++;
    flags.uHalfmoveClock = static_cast<u_int8_t>(std::atoi(&fen[index]));

    while (fen[index] >= '0' && fen[index] <= '9') index++;
    while (fen[index] == ' ') index++;

    flags.uMoves = static_cast<u_int16_t>(std::atoi(&fen[index]));

    return flags;
}


//String SaveFEN(const Flags flags, const u64f &bitboards);