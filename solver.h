#ifndef _MPPKRS_SOLVER_
#define _MPPKRS_SOLVER_

#include "board.h"
#include "state.h"
#include <stack>

class Solver {
public:
    /// create new solver
    Solver();
    
    /// destroy solver
    ~Solver();
    
    /// main method - Solve the board using DFS
    State* dfsSolve(Board& board);

private:
    bool pushPossibleMoves(State* curSt);
    bool* getPossibleMoves(Board& board, int x, int y);
    bool checkSolution(State* curSt);
    State* currentBest;
    std::stack<State*> dfsStack;
};

#endif