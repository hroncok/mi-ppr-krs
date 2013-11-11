#ifndef _MPPKRS_SOLVER_
#define _MPPKRS_SOLVER_

#include "board.h"
#include "state.h"
#include "logger.h"
#include <list>
#include <mpi.h>

// probe (=check) for incoming messages every PROBE_INTERVAL expanses
#define PROBE_INTERVAL 100

// message tags
#define MSG_WORK_REQUEST  1000
#define MSG_INCOMING_WORK 1001
#define MSG_WORK_DENIAL   1002
#define MSG_TOKEN         1003
#define MSG_FINISH        1004
#define MSG_BOARD         1005
#define MSG_RESULT        1006
#define MSG_ANY           1007

//color of the token or processor
#define COL_WHITE 0
#define COL_BLACK 1         

/// Class represents a "player" of the game.
/// Player may be master (pID == 0) or slave (pID != 0)
/// Master manages communication between players, starting/ending the game, etc.
/// Better solution would be an abstract class with the master and slave descendants
class Solver {
public:
    /// Create new player
    Solver(const Board& initB, int pCount, int pID);

    /// Destroy player
    ~Solver();

    /// Parallel solve
    /// Master returns the result, slave ends within this method
    State* Solve();    

private:
    /// Begin the computation, stop when dfsStack.size() == pCount
    /// Send work to all other processors
    void initMaster();
    
    /// Wait for work from the master
    void initSlave();
    
    /// Part of the solving, when processor is active
    /// DFS until finish message is received, the best possible solution is found or the stack is empty
    void procActive();
    
    /// Part of the solving, when processor is idle
    /// Send the token (if holding it), Ask for next work, process incoming messages
    void procIdle();
    
    /// One DFS expansion, same as in sequential application
    /// Pop the stack, check the board and push possible moves on stack
    void dfsExpand();
    
    /// Supportive method for dfsExpand, same as in sequential application
    /// check the board and push possible moves on stack
    bool pushPossibleMoves(State& curSt);
    
    /// Supportive method for dfsExpand, same as in sequential application
    /// true if the solution found is better than the local best
    bool isSolutionBest(int remaining) const;
    
    /// Main probing and receiving method
    /// May be blocking (wait for the message) or nonblocking (only check)
    /// May process only for given tag or source processor (default any tag, any source)
    /// True if the message is received, false if there is no message
    /// Received message is also processed (check the implementation)
    bool tryReceiveMessage(bool blocking, int mesTag = MPI_ANY_TAG, int mesSource = MPI_ANY_SOURCE);
    
    /// if there is enough states on stack -> send state to the processor dest
    /// currently sends only one state, should be improved
    /// note that messages should be smaller than 1kB (we do not want to use ISend)
    /// states are taken from the front of the stack (biggest expansion potential)
    bool trySendWork(int dest);
    
    /// if master: send finish request to all other processors, collect results, return the best result
    /// if slave: send finish request and the result (currentBest) to master, terminate.
    void finishParDfs();
    
    /// Master checks if received result is better than its local best
    /// if yes -> replace local best
    void checkResult(const unsigned char* serMoves);
    
    /// Received token -> resend or save it
    /// if master: finish or start new round with white token
    /// if slave: resend black token or save white token (send when become idle)
    void processToken(unsigned char tokColor);
    
    State* currentBest; // locally best solution found
    std::list<State*> dfsStack; //stack for the states of the game
    Board initBoard;  //initial Board
    int pCount; //total number of processors
    int pID; //my ID (rank)
    unsigned char color; //what is my color?
    bool holdingToken; //do I have the token at the moment?
    int donorID; // rank of possible donor of work (i will ask him when i become idle)   
    bool finished; //indicates if the computation finished and result can be returned, not really clear solution
    Logger* log; //pointer to the instance of logger
};

#endif