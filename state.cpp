#include <iostream>
#include "state.h"

State::State(const Board& initBoard, const unsigned char* serMoves) :
board(initBoard), //copy init Board
moveSeq(serMoves + 1, serMoves + serMoves[0]) {  //"auto deserialize" of moves
    for (int i = 0; i < (int)moveSeq.size(); i += 3) {
        //make moves on the copied init board
        int x = moveSeq[i];
        int y = moveSeq[i + 1];
        int dir = moveSeq[i + 2];

        this->board.makeMove(x, y, dir);
    }
}

bool State::makeMove(int x, int y, int dir) {
    if (!this->board.makeMove(x, y, dir)) return false;

    // add the move into the vector of moves
    this->moveSeq.push_back(x);
    this->moveSeq.push_back(y);
    this->moveSeq.push_back(dir); 

    return true;
}

int State::remains() const {
    return this->board.getPinCount();
}

const std::vector<unsigned char>& State::getMoves() const {
    return this->moveSeq;
}

unsigned char* State::getSerializedMoves() const {   
    int size = this->moveSeq.size() + 1;
    unsigned char* serData = new unsigned char[size];
    serData[0] = size; //save size in the first byte

    for (int i = 1; i < size; i++) {
        serData[i] = this->moveSeq[i - 1];
    }

    return serData;
}

const Board& State::getBoard() const {
    return this->board;
}
