#include "state.h"


State::State(Board& board) {
    this->board = new Board(board);
    this->moveSeq = new std::vector<unsigned char>;
}

State::State(State& oldState) {
    this->board = new Board(*(oldState.board));
    this-> moveSeq = new std::vector<unsigned char>(*(oldState.moveSeq));
}

State::~State() {
    delete this->board;
    delete this->moveSeq;
}

bool State::makeMove(int x, int y, int dir) {
    if (this->remains() <= 1) return false;
    this->board->move(x, y, dir);
   
    this->moveSeq->push_back(x);
    this->moveSeq->push_back(y);
    this->moveSeq->push_back(dir);
    
    return true;
}

int State::remains() {
    return this->board->pins();
}

std::vector<unsigned char>* State::getMoves() {
    return this->moveSeq;
}

Board* State::getBoard() {
    return this->board;
}



