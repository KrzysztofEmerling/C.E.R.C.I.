#include "UCI.h"

#include "Perft.h"
#include "Eval.h"

#include <chrono>

UniversalChessInterface::UniversalChessInterface() : positionIsSet(false), engineIsSherching(false), stopUCI(false), chessBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
}

void UniversalChessInterface::ExecuteInstructionLine(String line)
{
    std::stringstream ss(line);
    String token;
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
    else if (token == "d")
    {
        chessBoard.DrawBoard();
    }
    else if (token == "position")
    {
        handleSetPosition(ss);
    }
    else if (token == "go")
    {
        handleSherch(ss);
    }
    else if (token == "stop" && engineIsSherching)
    {
        Eval::StopSearch();
    }
    else if (token == "quit")
    {
        Eval::StopSearch();

        if (engineThread.joinable())
            engineThread.join();

        stopUCI = true;
    }
}

void UniversalChessInterface::handleSetPosition(std::stringstream &ss)
{
    String token;
    ss >> token;

    String fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::vector<String> moveList;

    if (token == "startpos")
    {
        cachedFEN = fen;
        if (ss >> token && token == "moves")
        {
            String moveToken;
            while (ss >> moveToken)
                moveList.push_back(moveToken);
        }
    }
    else if (token == "fen")
    {
        String line;
        std::getline(ss, line);
        line.erase(0, line.find_first_not_of(' '));

        std::size_t pos = line.find("moves");
        if (pos != String::npos)
        {
            cachedFEN = line.substr(0, pos);
            std::stringstream movesStream(line.substr(pos + 5));
            String moveToken;
            while (movesStream >> moveToken)
                moveList.push_back(moveToken);
        }
        else
        {
            cachedFEN = line;
        }
    }

    if (cachedFEN == fen && moveList.size() > cachedMoves.size() &&
        std::equal(cachedMoves.begin(), cachedMoves.end(), moveList.begin()))
    {
        for (size_t i = cachedMoves.size(); i < moveList.size(); ++i)
            chessBoard.MakeMove(moveList[i].c_str());

        cachedMoves = moveList;
        positionIsSet = true;
        return;
    }

    chessBoard = BoardState(cachedFEN.c_str());
    for (auto &m : moveList)
        chessBoard.MakeMove(m.c_str());

    cachedMoves = moveList;
    positionIsSet = true;
}

void UniversalChessInterface::handleSherch(std::stringstream &ss)
{
    if (engineThread.joinable())
        engineThread.join();
    int movetimeMs = -1;
    int depth = -1;

    String param;
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
            std::cout << "Depth selected: " << depth << std::endl;

            engineIsSherching = true;

            engineThread = std::thread([this, depth]()
                                       {
            Move bestMove = Eval::FindBestMoveFixedDepth(chessBoard, depth);
            std::cout << "bestmove " << toAlgebraicNotation(bestMove) << std::endl;
            engineIsSherching = false; });

            break;
        }
        else if (param == "perft")
        {
            ss >> depth;

            engineThread = std::thread([this, depth]()
                                       {
                        Perft(chessBoard, depth);
                        engineIsSherching = false; });

            break;
        }
    }

    if (depth == -1)
    {
        engineIsSherching = true;
        engineThread = std::thread([&]()
                                   {
                        Move bestMove = Eval::FindBestMove(chessBoard);
                        std::cout << "bestmove " << toAlgebraicNotation(bestMove) << std::endl;
                        engineIsSherching = false; });

        if (movetimeMs > 0)
        {
            std::thread timerThread([&, movetimeMs]()
                                    {
                            std::this_thread::sleep_for(std::chrono::milliseconds(movetimeMs));
                            Eval::StopSearch();
                            if (engineThread.joinable())
                                engineThread.join();
                            engineIsSherching = false; });
            timerThread.detach();
        }
    }
}

String UniversalChessInterface::toAlgebraicNotation(Move move)
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
        moveNotation += 'n';
        break;
    default:
        break;
    }

    return moveNotation;
}