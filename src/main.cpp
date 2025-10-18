#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>

#include <thread>
#include <chrono>

#include "BoardState.h"
#include "Eval.h"
#include "Perft.h"

String ToAlgebraicNotation(Move move)
{
    String moveNotation;
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
            std::cout << "id author Krzysztof Emerling" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (token == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (token == "ucinewgame")
        {
            positionIsSet = false;
            Eval::PrepareForNewGame();
        }

        if (token == "position")
        {
            std::string subtoken;
            ss >> subtoken;
            if (subtoken == "startpos")
            {
                chessBoard = BoardState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

                positionIsSet = true;
                ss >> subtoken;
                if (subtoken == "moves")
                {
                    std::string moveToken;
                    while (ss >> moveToken)
                    {
                        chessBoard.MakeMove(moveToken.c_str());
                    }
                }
            }
            else if (subtoken == "fen")
            {
                std::string fenString;
                std::getline(ss, fenString);
                fenString.erase(0, fenString.find_first_not_of(' '));

                chessBoard = BoardState(fenString.c_str());

                positionIsSet = true;
            }
        }

        if (token == "go")
        {
            if (engineThread.joinable())
                engineThread.join();
            int movetimeMs = -1;
            int depth = -1;

            std::string param;
            while (ss >> param)
            {
                if (param == "wtime")
                {
                    int wtime = 0, btime = 0, winc = 0, binc = 0;
                    ss >> wtime;
                    ss >> param;
                    ss >> btime;

                    ss >> param;
                    if (param == "winc")
                    {
                        ss >> winc;
                        ss >> param;
                        ss >> binc;
                    }
                    movetimeMs = Eval::CalculateTimeToSearch(chessBoard.GetMoves(), wtime, btime, winc, binc, chessBoard.IsWhiteMove());
                }
                else if (param == "movetime")
                {
                    ss >> movetimeMs;
                    break;
                }
                else if (param == "depth")
                {
                    ss >> depth;
                    engineIsRunning = true;

                    engineThread = std::thread([&]()
                                               {
                        Move bestMove = Eval::FindBestMoveFixedDepth(chessBoard, depth);
                        std::cout << "bestmove " << ToAlgebraicNotation(bestMove) << std::endl;
                        engineIsRunning = false; });
                    break;
                }
                else if (param == "perft")
                {
                    ss >> depth;

                    engineThread = std::thread([&]()
                                               {
                        Perft(chessBoard, depth);
                        engineIsRunning = false; });

                    break;
                }
            }

            if (depth == -1)
            {
                engineIsRunning = true;
                engineThread = std::thread([&]()
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
        else if (token == "d")
        {
            chessBoard.DrawBoard();
        }

        if (token == "quit")
        {
            if (engineThread.joinable())
                engineThread.join();
            break;
        }
    }
}
