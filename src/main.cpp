#include <iostream>
#include <cstdlib>

#include "BoardState.h"
#include "Eval.h"
#include "Perft.h"

int main()
{
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    // 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
    // pozycja dla prowadzącego: qqqqkqqq/qqqqqqqq/qqqqqqqq/qqqqqqqqq/8/8/PPPPPPPP/RNBQKBNR w KQ - 0 1"
    BoardState chessBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    while (true)
    {
        chessBoard.DrawBoard();

        std::string moveNotation;
        std::cout << "\nPodaj ruch (np. e2e4, e7e8q), \"p 6\" aby wykonać perft lub \"quit\" aby zakończyć: ";
        std::getline(std::cin, moveNotation);

        // Usuń białe znaki z początku i końca
        moveNotation.erase(0, moveNotation.find_first_not_of(" \t\n\r"));
        moveNotation.erase(moveNotation.find_last_not_of(" \t\n\r") + 1);

        if (moveNotation == "quit")
        {
            std::cout << "Gra zakończona." << std::endl;
            break;
        }
        else if (!moveNotation.empty() && moveNotation[0] == 'p')
        {
            // Obsługa polecenia "p N"
            std::string depthStr = moveNotation.substr(1);
            try
            {
                int depth = std::stoi(depthStr);
                Perft(chessBoard, depth);
            }
            catch (...)
            {
                std::cout << "Niepoprawna liczba dla perft!\n";
                continue;
            }
        }
        else if (moveNotation.size() != 4 && moveNotation.size() != 5)
        {
            std::cout << "Niepoprawny format ruchu! Spróbuj ponownie.\n";
            continue;
        }
        else
        {
            while (!chessBoard.MakeMove(moveNotation.c_str()))
            {
                std::cout << "Niepoprawny ruch! Spróbuj ponownie.\n";
                std::cin >> moveNotation;
            }

#ifdef _WIN32
            int val = system("cls");
#else
            int val = system("clear");
#endif
        }

        if (chessBoard.IsCheckmate())
        {
            chessBoard.DrawBoard();
            std::cout << "Szach-mat!\n";
            break;
        }
        else if (chessBoard.IsStalemate())
        {
            chessBoard.DrawBoard();
            std::cout << "Pat! Gra zakończona remisem.\n";
            break;
        }
        else if (chessBoard.IsFiftyMoveRule())
        {
            chessBoard.DrawBoard();
            std::cout << "Remis przez regułę 50 posunięć.\n";
            break;
        }
        else if (chessBoard.IsThreefoldRepetition())
        {
            chessBoard.DrawBoard();
            std::cout << "Remis przez trzykrotne powtórzenie pozycji.\n";
            break;
        }
        else if (chessBoard.IsInsufficientMaterial())
        {
            chessBoard.DrawBoard();
            std::cout << "Remis z powodu niewystarczającego materiału.\n";
            break;
        }

        // Ruch bota
        std::cout << "\nBot myśli...\n";
        Move botMove = Eval::FindBestMove(chessBoard, 6);

        // char f1 = 'a' + (botMove.startingSquere % 8);
        // char r1 = '1' + (botMove.startingSquere / 8);
        // char f2 = 'a' + (botMove.destSquere % 8);
        // char r2 = '1' + (botMove.destSquere / 8);

        // std::cout << "\nBot wykonuje: " << f1 << r1 << f2 << r2 << "\n";

        chessBoard.MakeMove(botMove);
        std::cout << "\nTwój ruch.\n";

        // Sprawdź zakończenie gry po ruchu bota
        if (chessBoard.IsCheckmate() || chessBoard.IsStalemate() || chessBoard.IsFiftyMoveRule() ||
            chessBoard.IsThreefoldRepetition() || chessBoard.IsInsufficientMaterial())
        {
            chessBoard.DrawBoard();
            std::cout << "Koniec gry.\n";
            break;
        }
    }
}
