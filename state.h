#ifndef _MPPKRS_STATE_
#define	_MPPKRS_STATE_

#include "board.h"
#include <vector>

/// Class State represents one concrete position during the game
/// It is defined by the board state and moves done
class State {
public:
    /// Create initial (first) state of the game
    /// No moves have been done yet
    State(const Board& oldBoard) : board(oldBoard) {
    }

    /// Create new State as a copy of another State
    /// After creation, move is done by makeMove(x,y)
    /// Almost all States are supposed to be created this way (excluding initial and sent states)
    State(const State& oldState) : board(oldState.board), moveSeq(oldState.moveSeq) {
    }

    /// Create new State from serialized moves
    /// Deserializes moves, makes all these moves on copied init board
    /// All states received from another processor are supposed to be created this way
    State(const Board& initBoard, const unsigned char* serMoves);
    

    /// Update state with a new move
    bool makeMove(int x, int y, int dir);

    /// Get count of pins remaining on the board
    int remains() const;

    /// Get vector of bytes with all done moves
    /// vector looks like this:
    /// (x1),(y1),(dir1),(x2),(y2),(dir2), ...
    const std::vector<unsigned char>& getMoves() const;
    
    /// Get serialized vector of moves in the form of byte array
    /// serialized array looks like this:
    /// (size of this array),(x1),(y1),(dir1),(x2),(y2),(dir2), ...
    unsigned char* getSerializedMoves() const;

    /// Get current board
    const Board& getBoard() const;

private:
    Board board; //current board
    std::vector<unsigned char> moveSeq; //vector of moves

};

#endif

