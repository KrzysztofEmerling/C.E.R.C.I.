#include <iostream>
#include <cstdlib>

#include "BoardState.h"
#include "Eval.h"
#include "Perft.h"

int main()
{
    BoardState chessBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    while (true)
    {
#ifdef _WIN32
        int val = system("cls");
#else
        int val = system("clear");
#endif

        chessBoard.DrawBoard();

        if (chessBoard.IsCheckmate())
        {
            std::cout << "\nSzach-mat! ";
            std::cout << (chessBoard.GetFlags().whiteOnMove ? "Czarne" : "Białe") << " wygrały!\n";
            break;
        }
        else if (chessBoard.IsStalemate())
        {
            std::cout << "\nPat! Gra zakończona remisem.\n";
            break;
        }
        else if (chessBoard.IsFiftyMoveRule())
        {
            std::cout << "\nRemis przez regułę 50 posunięć.\n";
            break;
        }
        else if (chessBoard.IsThreefoldRepetition())
        {
            std::cout << "\nRemis przez trzykrotne powtórzenie pozycji.\n";
            break;
        }
        else if (chessBoard.IsInsufficientMaterial())
        {
            std::cout << "\nRemis z powodu niewystarczającego materiału.\n";
            break;
        }

        std::cout << "\n"
                  << (chessBoard.GetFlags().whiteOnMove ? "Białe" : "Czarne") << " (Bot) myślą...\n";

        Move botMove = Eval::FindBestMove(chessBoard, 6);

        char f1 = 'a' + (botMove.startingSquere % 8);
        char r1 = '1' + (botMove.startingSquere / 8);
        char f2 = 'a' + (botMove.destSquere % 8);
        char r2 = '1' + (botMove.destSquere / 8);

        std::cout << "Ruch: " << f1 << r1 << f2 << r2 << "\n";

        chessBoard.MakeMove(botMove);

        // Można dodać krótkie opóźnienie, aby nie migało zbyt szybko (opcjonalnie)
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
