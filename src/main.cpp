#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>

#include <thread>
#include <chrono>

#include "BoardState.h"
#include "Eval.h"
#include "Perft.h"

std::string ToAlgebraicNotation(Move move)
{
    std::string moveNotation;
    moveNotation.push_back('a' + (move.startingSquere % 8));
    moveNotation.push_back('1' + (move.startingSquere / 8));
    moveNotation.push_back('a' + (move.destSquere % 8));
    moveNotation.push_back('1' + (move.destSquere / 8));

    switch (move.flag)
    {
    case MovesFlags::PromotionQueen:
        moveNotation += 'q';
        break;
    case MovesFlags::PromotionRook:
        moveNotation += 'r';
        break;
    case MovesFlags::PromotionBishop:
        moveNotation += 'b';
        break;
    case MovesFlags::PromotionKnight:
        moveNotation += 'k';
        break;
    default:
        break;
    }

    return moveNotation;
}

int main()
{
    std::string line;
    bool positionIsSet = false;
    bool engineIsRunning = false;
    BoardState chessBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::thread engineThread;

    std::size_t posAt;
    while (std::getline(std::cin, line))
    {
        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "uci")
        {
            std::cout << "id name CERCI" << std::endl;
            std::cout << "id author KrzysztofEmerling" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (token == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (token == "ucinewgame")
        {
            positionIsSet = false;
        }

        if (!positionIsSet)
        {
            if (token == "position")
            {
                std::string subtoken;
                ss >> subtoken;
                if (subtoken == "startpos")
                {
                    positionIsSet = true;
                }
                else if (subtoken == "fen")
                {
                    std::string fenString, fenPart;
                    std::getline(ss, fenString);
                    fenString.erase(0, fenString.find_first_not_of(' '));

                    chessBoard.SetFen(fenString);
                    positionIsSet = true;
                }
            }
        }
        else
        {
            if (token == "go")
            {
                int movetimeMs = -1;
                int depth = -1;

                std::string param;
                while (ss >> param)
                {
                    if (param == "movetime")
                    {
                        ss >> movetimeMs;
                    }
                    else if (param == "depth")
                    {
                        ss >> depth;
                    }
                }

                // jeśli jakiś wątek już działa, dołącz go
                if (engineThread.joinable())
                    engineThread.join();
                engineIsRunning = true;

                if (depth > 0)
                {
                    engineThread = std::thread([&, movetimeMs]()
                                               {
                        Move bestMove = Eval::FindBestMoveFixedDepth(chessBoard, depth);
                        std::cout << "bestmove " << ToAlgebraicNotation(bestMove) << std::endl;
                        engineIsRunning = false; });
                }
                else
                {
                    engineThread = std::thread([&, movetimeMs]()
                                               {
                        Move bestMove = Eval::FindBestMove(chessBoard);
                        std::cout << "bestmove " << ToAlgebraicNotation(bestMove) << std::endl;
                        engineIsRunning = false; });

                    if (movetimeMs > 0)
                    {
                        std::thread timerThread([&, movetimeMs]()
                                                {
                            std::this_thread::sleep_for(std::chrono::milliseconds(movetimeMs));
                            Eval::StopSearch();
                            if (engineThread.joinable())
                                engineThread.join();
                            engineIsRunning = false; });
                        timerThread.detach();
                    }
                }
            }
            else if (token == "stop" && engineIsRunning)
            {
                Eval::StopSearch();
            }
        }

        if (token == "quit")
        {
            if (engineThread.joinable())
                engineThread.join();
            break;
        }
    }
}
