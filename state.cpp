#include "state.h"

bool State::makeMove(int x, int y, int dir) {    
    if (!this->board.makeMove(x,y,dir)) return false;

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

const Board& State::getBoard() const {
    return this->board;
}



