#include "solver.h"

Solver::Solver() {
}

State* Solver::dfsSolve(Board& board) {
    this->currentBest = NULL;
    dfsStack.push(new State(board)); // create initial state

    while (!(this->dfsStack.empty())) {
        State* curSt = this->dfsStack.top();
        this->dfsStack.pop();

        if (pushPossibleMoves(curSt)) {
            delete curSt;
        } else if (checkSolution(curSt)) {
            return this->currentBest;
        }
    }
    return this->currentBest;
}

bool Solver::pushPossibleMoves(State* curSt) {
    bool pushed = false;
    int height = curSt->getBoard()->getHeight();
    int width = curSt->getBoard()->getWidth();

    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            if (curSt->getBoard()->is_on(x, y)) {
                bool* moves = getPossibleMoves(*(curSt->getBoard()), x, y);
                for (int i = 0; i < 4; i++) {
                    if (moves[i]) {
                        State* newSt = new State(*curSt);
                        newSt->makeMove(x, y, i);
                        this->dfsStack.push(newSt);
                    }
                }
                delete moves;
            }
        }
    }
    return pushed;
}

bool* Solver::getPossibleMoves(Board& board, int x, int y) {
    bool* moves = new bool[4];
    std::fill(moves, moves + sizeof (moves), false);

    if ((x > 1) && (board.is_on(x - 1, y)) && (!board.is_on(x - 2, y))) moves[NORTH] = true;
    if ((y < board.getWidth() - 1) && (board.is_on(x, y + 1))&& (!board.is_on(x, y + 2))) moves[EAST] = true;
    if ((x < board.getHeight() - 1) && (board.is_on(x + 1, y))&& (!board.is_on(x + 2, y))) moves[SOUTH] = true;
    if ((y > 1) && (board.is_on(x, y - 1))&& (!board.is_on(x, y - 2))) moves[WEST] = true;

    return moves;
}

bool Solver::checkSolution(State* curSt) {
    // there is no move possible   
    if (this->currentBest == NULL) {
        this->currentBest = curSt;
    } else if ((curSt->remains()) < (this->currentBest->remains())) {
        delete this->currentBest;
        this->currentBest = curSt;
    } else {
        delete curSt;
    }

    return (this->currentBest->remains() == 1);
}

Solver::~Solver() {
    if (this->currentBest) delete this->currentBest;
}

