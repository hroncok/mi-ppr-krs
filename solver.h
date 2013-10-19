#ifndef _MPPKRS_SOLVER_
#define _MPPKRS_SOLVER_

#include "board.h"
#include "state.h"
#include <stack>

class Solver {
public:
    /// create new solver
    Solver() {}

    /// destroy solver
    ~Solver();

    /// main method - Solve the board using DFS
    State* dfsSolve(const Board& board);

private:
    bool pushPossibleMoves(State& curSt);
    bool* getPossibleMoves(Board& board, int x, int y) const;
    bool isSolutionBest(int remaining) const;
    void replaceBest(State* curSt);
    State* currentBest;
    std::stack<State*> dfsStack;
};

#endif