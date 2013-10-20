#include "solver.h"
#include <iostream>

State* Solver::dfsSolve(const Board& board) {
    this->currentBest = NULL;
    dfsStack.push(new State(board)); // create initial state

    while (!(this->dfsStack.empty())) {
        State* curSt = this->dfsStack.top();
        this->dfsStack.pop();

        if (pushPossibleMoves(*curSt)) {
            delete curSt;
        } else if (isSolutionBest(curSt->remains())) {
            replaceBest(curSt);
            if (curSt->remains() == 1) break;
        } else {
            delete curSt;
        }
    }
    return new State(*this->currentBest);
}

bool Solver::pushPossibleMoves(State& curSt) {
    bool pushed = false;
    int height = curSt.getBoard().getHeight();
    int width = curSt.getBoard().getWidth();

    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            if (curSt.getBoard().isPinOn(x, y)) {
                for (int i = 0; i < 4; i++) {
                    if (curSt.getBoard().isPossibleMove(x, y, i)) {
                        State* newSt = new State(curSt);
                        newSt->makeMove(x, y, i);
                        this->dfsStack.push(newSt);
                        pushed = true;                         
                    }
                }
            }
        }
    }
    return pushed;
}

bool Solver::isSolutionBest(int remaining) const {
    // there is no move possible   
    if (this->currentBest == NULL) return true;
    if ((remaining) < (this->currentBest->remains())) return true;

    return false;
}

void Solver::replaceBest(State* newBest) {
    if (this->currentBest) delete this->currentBest;
    this->currentBest = newBest;
}

Solver::~Solver() {
    if (this->currentBest) delete this->currentBest;
}

