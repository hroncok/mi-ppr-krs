#ifndef _MPPKRS_STATE_
#define	_MPPKRS_STATE_

#include "board.h"
#include <vector>

/// Class State represents one concrete position during the game
/// It is defined by the board state and moves done

class State {
public:
    // create initial (first) state of the game
    // no moves have been done yet
    State(const Board& oldBoard) : board(oldBoard) {
    }

    /// create new State as a copy of another State
    /// All states (during the game) are supposed to be created this way
    State(const State& oldState) : board(oldState.board), moveSeq(oldState.moveSeq) {
    }

    // update state with a new move
    bool makeMove(int x, int y, int dir);

    // return count of pins remaining on the board
    int remains();

    // return vector with all done moves
    const std::vector<unsigned char>& getMoves() const;

    // return current state of the board
    Board& getBoard();

private:
    Board board;
    std::vector<unsigned char> moveSeq;

};

#endif

