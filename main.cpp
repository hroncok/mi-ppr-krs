#include <cstdlib>
#include <iostream>
#include <vector>
#include "board.h"
#include "solver.h"
#include "state.h"

/// Check if arguments are correct, not completed
bool checkArgs(int argc, char** argv) {
    if (argc != 4) return false;
    // perform all necessary argument checks
    // ...
    return true;
}

/// visualize all moves of the solution (result)
void visualizeResult(Board& initBoard, std::vector<unsigned char>& moves) {
    int moveCount = moves.size() / 3;
    std::cout << "Visualized moves: " << std::endl;
    for (int i = 0; i < (moveCount * 3) - 2; i += 3) {
        int x = moves[i];
        int y = moves[i + 1];
        int dir = moves[i + 2];

        initBoard.visualize();
        std::cout << std::endl;
        initBoard.move(x, y, dir);
    }
    initBoard.visualize();
}

/// print all moves of the solution in the form of "[x,y] -> direction code"
void printResult(std::vector<unsigned char>& moves) {
    int moveCount = moves.size() / 3;
    std::cout << "Total moves done: " << moveCount << std::endl;
    for (int i = 0; i < (moveCount * 3) - 2; i += 3) {
        int x = moves[i];
        int y = moves[i + 1];
        int dir = moves[i + 2];

        std::cout << "[" << x << "," << y << "] -> " << dir << std::endl;
    }

}

int main(int argc, char** argv) {
    //if (checkArgs == false) return 1;
    //Board game(argv[0], argv[1], argv[2], argv[3]);

    Board game(5, 5, 10, 20); // create sample game
    std::cout << "Initial state:" << std::endl;
    game.visualize();

    Solver player; // create a player (solver)
    State* result = player.dfsSolve(game); // play the game
    std::cout << std::endl << "Final state:" << std::endl;
    result->getBoard()->visualize();

    std::cout << std::endl;
    printResult(*(result->getMoves())); 
    //std::cout << std::endl;
    //visualizeResult(game,*(result->getMoves()));
    
    return 0;
}

