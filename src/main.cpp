#include<iostream>
#include <cstdlib> 

#include "MoveGenerator/BoardState.h"



int main() {
    BoardState chessBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    while (true) {
        // Rysowanie planszy
        chessBoard.DrawBoard();


        String move_notation;
        std::cout << "\nPodaj ruch (np. e2e4) lub wpisz #end, aby zakończyć: ";
        std::cin >> move_notation;
        if (move_notation == "#end") {
            std::cout << "Gra zakończona." << std::endl;
            break;
        }

        if (move_notation.size() != 4) {
            std::cout << "Niepoprawny format ruchu! Spróbuj ponownie.\n";
            continue;
        }
        chessBoard.MakeMove(ParseMove(move_notation.c_str()));

        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    return 0;
}