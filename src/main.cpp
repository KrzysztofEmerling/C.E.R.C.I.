#include<iostream>
#include <cstdlib> 

#include "MoveGenerator/BoardState.h"



int main() {
    //rnbqkb1r/1p2pppp/p2p1n2/8/3NP3/2N5/PPP2PPP/R1BQKB1R w KQkq – 0 1
    //rnbqkb1r/1p2pppp/p2p1n2/8/3NP3/2N5/PPP2PPP/R1BQKB1R w KQkq - 1 3
    BoardState chessBoard("rnbqkb1r/1p2pppp/p2p1n2/8/3NP3/2N5/PPP2PPP/R1BQKB1R w KQkq - 1 3");

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