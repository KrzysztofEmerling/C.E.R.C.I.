#include <iostream>
#include <cstdlib>

#include "MoveGenerator/BoardState.h"
#include "Debuging/Debug.h"

int main()
{
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    // 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1
    BoardState chessBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

    while (true)
    {
        // Rysowanie planszy
        chessBoard.DrawBoard();

        String moveNotation;
        std::cout << "\nPodaj ruch (np. e2e4), \"p 6\" żeby wypisać podzielone ruchy lub wpisz end, aby zakończyć: ";
        std::cin >> moveNotation;
        if (moveNotation == "end")
        {
            std::cout << "Gra zakończona." << std::endl;
            break;
        }
        else if (moveNotation[0] == 'p')
        {
            String numberStr = moveNotation.substr(1, 3);
            Debug::Perft(chessBoard, std::stoi(numberStr));
        }
        else if (moveNotation.size() != 4)
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
            };

            // #ifdef _WIN32
            //             system("cls");
            // #else
            //             system("clear");
            // #endif
        }
    }

    return 0;
}