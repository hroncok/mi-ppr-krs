#include "state.h"

bool State::makeMove(int x, int y, int dir) {
    if (this->remains() <= 1) return false;
    this->board.move(x, y, dir);

    this->moveSeq.push_back(x);
    this->moveSeq.push_back(y);
    this->moveSeq.push_back(dir);

    return true;
}

int State::remains() {
    return this->board.pins();
}

const std::vector<unsigned char>& State::getMoves() const {
    return this->moveSeq;
}

Board& State::getBoard() {
    return this->board;
}



