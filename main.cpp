#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <mpi.h>
#include "board.h"
#include "solver.h"
#include "state.h"
#include "logger.h"


/// print all moves of the solution in the form of "[x,y] -> direction code
void printResult(const std::vector<unsigned char>& moves) {
    int moveCount = moves.size() / 3;
    (*Logger::getInstance()) << "Total moves done: " << moveCount << "\n";
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
        (*Logger::getInstance()) << "[" << x << "," << y << "] -> " << dirStr << "\n";
    }
}

/// recieve the serialized board from the master
unsigned char* slaveReceiveSerBoard() {
    MPI_Status status;
    MPI_Probe(0, MSG_BOARD, MPI_COMM_WORLD, &status); //blocking wait for the board
    int mesSize;
    MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &mesSize); //get message length
    unsigned char* buf = new unsigned char[mesSize];

    //save the message into buf
    MPI_Recv(buf, mesSize, MPI_UNSIGNED_CHAR, 0, MSG_BOARD, MPI_COMM_WORLD, &status);

    return buf;
}

/// send the board to all slaves
void masterSendBoard(const Board& game, int pCount) {
    unsigned char* buf = game.getSerializedBoard(); //serialize board into a byte buffer
    for (int i = 1; i < pCount; i++) {
        //send the board to all other processes
        MPI_Send(buf, buf[3] + 4, MPI_UNSIGNED_CHAR, i, MSG_BOARD, MPI_COMM_WORLD);
    }
    delete[] buf;
}

int main(int argc, char** argv) {
    int pCount, pID;
    MPI_Init(&argc, &argv); //initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &pID); //get ID of this processor (rank)
    MPI_Comm_size(MPI_COMM_WORLD, &pCount); //get total number of processors

    Logger* log = Logger::getInstance(pID, true); //initialize instance of Logger class (singleton)

    //if (argc != 5) {
    if (argc != 2) {
        (*log) << "Invalid number of arguments\n";
        MPI_Finalize();
        exit(1);
    }
    //parse arguments
    int m = atoi(argv[1]);
    //int n = atoi(argv[2]);
    //int x = atoi(argv[3]);
    //int q = atoi(argv[4]);


    if (pID == 0) { // main process (master)
        //Board game(m, n, x, q); // create sample game
        Board game(m); // create test game
        masterSendBoard(game, pCount);
        (*log) << "Initial state:\n";
        game.visualize();
        double t1 = MPI_Wtime();
        Solver player(game, pCount, pID); // create master player
        State* result = player.Solve(); //parallel Solve
        double t2 = MPI_Wtime();
        (*log) << t2-t1;
        (*log) << "\nFinal state:\n";
        result->getBoard().visualize();
        (*log) << "\n";
        printResult(result->getMoves());
        delete result;


    } else { //other processes (slaves)
        unsigned char* buf = slaveReceiveSerBoard();
        Board game(buf); //deserialize received board
        delete[] buf;
        Solver player(game, pCount, pID); //create slave player
        player.Solve(); //parallel Solve
    }

    delete log->getInstance(); //delete logger's only instance
    MPI_Finalize();
    return 0;
}

