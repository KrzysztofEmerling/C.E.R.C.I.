#include <iostream>
#include <cstdlib>

#include "MoveGenerator/BoardState.h"
#include "Debuging/Debug.h"

int main()
{

    BoardState chessBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

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
            chessBoard.MakeMove(Move(SquareIndex[moveNotation[0] - 'a'][moveNotation[1] - '1'], SquareIndex[moveNotation[2] - 'a'][moveNotation[3] - '1']));

#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
        }
    }

    return 0;
}