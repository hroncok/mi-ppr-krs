#ifndef _MPPKRS_STATE_
#define	_MPPKRS_STATE_

#include "board.h"
#include <vector>

/// Class State represents one concrete position during the game
/// It is defined by the board state and moves done
class State {
public:
    // create inital (first) state of the game
    // no moves have been done yet
    State(Board& board);
    
    /// create new State as a copy of another State
    /// All states (during the game) are supposed to be created this way
    State(State& oldState);
    
    // destroy the state
    ~State();
    
    // update state with a new move
    bool makeMove(int x, int y, int dir);
    
    // return count of pins remaining on the board
    int remains();
    
    // return vector with all done moves
    std::vector<unsigned char>* getMoves();

    // return current state of the board
    Board* getBoard();
    
private:
    std::vector<unsigned char>* moveSeq;
    Board* board;

};

#endif

