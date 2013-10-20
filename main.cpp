#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "board.h"
#include "solver.h"
#include "state.h"



/// visualize all moves of the solution (result)
void visualizeResult(const Board& initBoard, const std::vector<unsigned char>& moves) {
    Board board(initBoard);
    int moveCount = moves.size() / 3;
    std::cout << "Visualized moves: " << std::endl;
    for (int i = 0; i < (moveCount * 3) - 2; i += 3) {
        int x = moves[i];
        int y = moves[i + 1];
        int dir = moves[i + 2];

        board.visualize();
        std::cout << std::endl;
        board.makeMove(x, y, dir);
    }
    board.visualize();
}

/// print all moves of the solution in the form of "[x,y] -> direction code"
void printResult(const std::vector<unsigned char>& moves) {
    int moveCount = moves.size() / 3;
    std::cout << "Total moves done: " << moveCount << std::endl;
    for (int i = 0; i < (moveCount * 3) - 2; i += 3) {
        int x = moves[i];
        int y = moves[i + 1];
        int dir = moves[i + 2];

        std::string dirStr;
        switch (dir) {
            case NORTH:
                dirStr = "UP";
                break;
            case EAST:
                dirStr = "RIGHT";
                break;
            case SOUTH:
                dirStr = "DOWN";
                break;
            case WEST:
                dirStr = "LEFT";
                break;
            default:
                dirStr = "ERR";
                break;
        }
        std::cout << "[" << x << "," << y << "] -> " << dirStr << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    int x = atoi(argv[3]);
    int q = atoi(argv[4]);

    Board game(m, n, x, q); // create sample game
    std::cout << "Initial state:" << std::endl;
    game.visualize();

    Solver player; // create a player (solver)
    State* result = player.dfsSolve(game); // play the game
    std::cout << std::endl << "Final state:" << std::endl;
    result->getBoard().visualize();

    std::cout << std::endl;
    printResult(result->getMoves());
    //std::cout << std::endl;
    //visualizeResult(game,result->getMoves());

    delete result;
    return 0;
}

