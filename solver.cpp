#include "solver.h"
#include "logger.h"
#include <iostream>
#include <cstdlib>

Solver::Solver(const Board& initB, int pCount, int pID) : initBoard(initB) {
    this->currentBest = NULL; //no solution found yet
    this->pCount = pCount; //number of processors
    this->pID = pID; //my rank (ID)
    this->color = COL_WHITE; //start as white
    this->donorID = (pID + 1) % pCount; //who will i ask for the work (when i will need some)
    this->finished = false;
    this->log = Logger::getInstance(); //get singleton's instance for easier usage

    this->holdingToken = (pID == 0); //master is holding token at the begining
}

Solver::~Solver() {
    if (this->currentBest)
        delete this->currentBest;
}

State* Solver::Solve() {
    if (this->pID == 0) {
        initMaster(); //start solving, stop when there is work for all other processors
        if (finished) return new State(*currentBest); //if there is not enough work
    } else {
        initSlave(); // wait for the work from the master
    }
   
    while (1) {
        // Processor has become active        
        procActive(); //DFS until empty stack or finish request received
        
        // Processor has become idle
        if (finished || pCount == 1) return new State(*currentBest); //did i receive finish request during DFS?                           
        
        procIdle(); // send token (if holding), ask for the work
        if (finished) return new State(*currentBest); //did i receive finish request when idle?
    }

    return NULL; //this will never happen
}

void Solver::initMaster() {
    dfsStack.push_back(new State(initBoard)); // create and push initial state

    (*log) << pID << ": computation started\n"; //DEBUG
    while ((((int) dfsStack.size()) < pCount) && (!dfsStack.empty())) {
        dfsExpand();
    }

    if (dfsStack.empty()) {
        (*log) << pID << ": there is not enough work for other processes (empty stack)\n"; //DEBUG
        finishParDfs();
    } else {
        for (int i = 1; i < pCount; i++) trySendWork(i); //send work to all other processors
    }
}

void Solver::initSlave() {
    (*log) << pID << ": board received, waiting for some work\n"; //DEBUG
    while (dfsStack.empty()) { //waiting for work from the master
        tryReceiveMessage(true);
    }
}

void Solver::procActive() {
    (*log) << pID << ": processor has became active\n"; //DEBUG
    int counter = 1;

    while (!dfsStack.empty()) {
        //DFS until received finish message / found best solution / empty stack
        if (((counter++) % PROBE_INTERVAL) == 0) {
            //Probe, receive and process the incoming messages
            counter = 1; // restart the counter
            tryReceiveMessage(false); //non-blocking check
            if (finished) return; // did i received finish request?
        }
        dfsExpand(); //one step (expansion) of DFS
        if ((currentBest) && (currentBest->remains() == 1)) {
            //if i have found solution with 1 pin remaining                
            (*log) << pID << ": hooray, found the best possible solution (1 pin remaining)\n"; //DEBUG
            finishParDfs(); //send finish request to master (if i am slave) or to all slaves (if i am master)
            return;
        }
    }
}

void Solver::procIdle() {
    (*log) << pID << ": processor became idle\n"; //DEBUG
    this->color = COL_WHITE; //all idle processors become white
    
    if (holdingToken) { //(re)send token
        (*log) << pID << ": sending white token\n"; //DEBUG
        // if master: start a new round with white token, if slave: send white token (black would be resend immediately after receiving)
        MPI_Send(&color, 1, MPI_UNSIGNED_CHAR, (pID + 1) % pCount, MSG_TOKEN, MPI_COMM_WORLD); //message is only 1 byte long (color)
        holdingToken = false;
    }

    // ask for next work
    (*log) << pID << ": asking proc " << donorID << " for the next work\n"; //DEBUG
    MPI_Send(NULL, 0, MPI_UNSIGNED_CHAR, this->donorID, MSG_WORK_REQUEST, MPI_COMM_WORLD); //empty message (request only)
    donorID = (donorID + 1) % pCount; //set the next potential donor
    if (donorID == pID) donorID = (donorID + 1) % pCount; //i cannot deal work to myself

    while (dfsStack.empty()) {
        // process incoming messages until some work or finish request will come
        tryReceiveMessage(true); //blocking waiting for the message
        if (finished) return; //did i receive finish message?
    }
}

void Solver::dfsExpand() {
    State* curSt = this->dfsStack.back();
    this->dfsStack.pop_back();

    bool pushed = pushPossibleMoves(*curSt);
    if ((!pushed) && (isSolutionBest(curSt->remains()))) {
        // replace best
        if (this->currentBest) delete this->currentBest;
        this->currentBest = curSt;
    } else {
        delete curSt;
    }
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
                        this->dfsStack.push_back(newSt);
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
#ifdef DEBUG
    std::cout << "endbranch: " << remaining << std::endl;
#endif
    if (this->currentBest == NULL) return true;
    if ((remaining) < (this->currentBest->remains())) return true;
    return false;
}

bool Solver::tryReceiveMessage(bool blocking, int mesTag, int mesSource) {
    MPI_Status status;

    // all messages that not match tag mesTag and source mesSource are ignored by this method
    // all ignored messages remains in "mailbox", can be processed later
    // by default, all messages are processed

    if (blocking) {
        MPI_Probe(mesSource, mesTag, MPI_COMM_WORLD, &status); //wait for the message
    } else {
        int flag;
        MPI_Iprobe(mesSource, mesTag, MPI_COMM_WORLD, &flag, &status); //check the "mailbox" and set the flag
        if (flag == 0) return false; // return false if there is no messages
    }

    // Message has arrived
    int mesSize;
    MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &mesSize); //get size of the message

    unsigned char* buf;
    if (mesSize > 0) { //allocate buffer if the message is not empty
        buf = new unsigned char[mesSize];
    } else {
        buf = NULL;
    }

    //receive and copy the message into the buffer
    MPI_Recv(buf, mesSize, MPI_UNSIGNED_CHAR, mesSource, mesTag, MPI_COMM_WORLD, &status);

    State* newSt;
    switch (status.MPI_TAG) {
        case MSG_WORK_REQUEST:
            (*log) << pID << ": received work request from proc " << status.MPI_SOURCE << "\n"; //DEBUG
            trySendWork(status.MPI_SOURCE); //send requested work or work-denial message
            break;
        case MSG_INCOMING_WORK:
            (*log) << pID << ": received new work from proc " << status.MPI_SOURCE << "\n"; //DEBUG
            newSt = new State(initBoard, buf); //deserialize received moves (by appropriate State constructor)
            dfsStack.push_front(newSt); //push received state on stack
            break;
        case MSG_WORK_DENIAL:
            (*log) << pID << ": received work denial from proc " << status.MPI_SOURCE << "\n"; //DEBUG
            (*log) << pID << ": asking proc " << donorID << " for the next work\n"; //DEBUG
            MPI_Send(NULL, 0, MPI_UNSIGNED_CHAR, donorID, MSG_WORK_REQUEST, MPI_COMM_WORLD); //ask again someone else
            donorID = (donorID + 1) % pCount; //set the next potential donor
            if (donorID == pID) donorID = (donorID + 1) % pCount; //i cannot deal work to myself
            break;
        case MSG_TOKEN:
            if (buf[0] == 0) (*log) << pID << ": received white token\n"; //DEBUG
            else (*log) << pID << ": received black token\n";
            processToken(buf[0]); //resend or save the token (or finish)
            break;
        case MSG_FINISH:
            (*log) << pID << ": received finish request from proc " << status.MPI_SOURCE << "\n"; //DEBUG
            finishParDfs(); //send finish request to master (if i am slave) or to all slaves (if i am master)
            break;
        case MSG_RESULT:
            (*log) << pID << ": received result from proc " << status.MPI_SOURCE << "\n"; //DEBUG
            //only master will receive results (after finish request)
            checkResult(buf); //check if the received result is better than local best
            break;
        default:
            //ignore
            (*log) << pID << "; received unknown message from proc " << status.MPI_SOURCE << "\n"; //DEBUG
            break;
    }
    if (buf) delete[] buf; //delete the allocated buffer
    return true;
}

bool Solver::trySendWork(int dest) {
    //Currently, only one state is send, should be improved
    //Check solver.h for method description

    if ((this->dfsStack.size()) < 2) {
        //i have not enough work to send
        (*log) << pID << ": sending work denial to proc " << dest << " (" << dfsStack.size() << " items on stack)\n"; //DEBUG
        MPI_Send(NULL, 0, MPI_UNSIGNED_CHAR, dest, MSG_WORK_DENIAL, MPI_COMM_WORLD); //empty message, just work denial
        return false;
    }

    // I have enough work to share :)
    unsigned char* serMoves = this->dfsStack.front()->getSerializedMoves(); //serialize state from the FRONT of the stack
    delete this->dfsStack.front();
    this->dfsStack.pop_front(); //remove state from the stack


    (*log) << pID << ": sending new work to proc " << dest << " (" << dfsStack.size() << " items on stack)\n"; //DEBUG
    MPI_Send(serMoves, serMoves[0], MPI_UNSIGNED_CHAR, dest, MSG_INCOMING_WORK, MPI_COMM_WORLD); //send the work

    //if the rank of the destination processor is lower than mine, change color to black
    if ((pID != 0) && (dest < (this->pID))) this->color = COL_BLACK;
    return true;
}

void Solver::finishParDfs() {
    // i want to finish
   
    if (this->pID == 0) { //master
        // received finish request frome one of the slaves
        finished = true;
        for (int i = 1; i < pCount; i++) {
            // send finish request to all processors
            (*log) << pID << ": sending finish request to proc " << i << "\n"; //DEBUG
            MPI_Send(NULL, 0, MPI_UNSIGNED_CHAR, i, MSG_FINISH, MPI_COMM_WORLD);
        }

        //collect results from all slaves
        for (int i = 1; i < pCount; i++) {
            tryReceiveMessage(true, MSG_RESULT); //is the received better than local best?
        }

    } else { //slave
        // send finish request to the master
        (*log) << pID << ": sending finish request to proc 0\n"; //DEBUG
        MPI_Send(NULL, 0, MPI_UNSIGNED_CHAR, 0, MSG_FINISH, MPI_COMM_WORLD);
        if (currentBest) {
            //send master also the result (local best)
            unsigned char* buf = currentBest->getSerializedMoves(); //get serialized result (local best)
            (*log) << pID << ": sending result to proc 0 (" << currentBest->remains() << " pins remaining)\n"; //DEBUG
            MPI_Send(buf, buf[0], MPI_UNSIGNED_CHAR, 0, MSG_RESULT, MPI_COMM_WORLD);
            delete[] buf;
        } else {
            //I don't have any local solution, send master "empty result" message
            (*log) << pID << ": sending empty result (i have not found any) to proc 0\n"; //DEBUG
            MPI_Send(NULL, 0, MPI_UNSIGNED_CHAR, 0, MSG_RESULT, MPI_COMM_WORLD);
        }

        //finish request and result were sent to master, terminate!
        MPI_Finalize();
        exit(0);
    }
}

void Solver::checkResult(const unsigned char* serMoves) {
    if ((!serMoves) || (pID != 0) || (!finished)) return;

    if (currentBest) {
        //is the received result better than my local best?
        if (((int) currentBest->getMoves().size()) < (serMoves[0] - 1)) {
            // yes, it is better
            delete currentBest;
            currentBest = new State(initBoard, serMoves);
        }
    } else {
        //i don't have any local result, save the received one
        currentBest = new State(initBoard, serMoves);
    }
}

void Solver::processToken(unsigned char tokColor) {
    holdingToken = true;
    if (pID == 0) { //master        
        if (tokColor == COL_WHITE) {
            // master received white token, game is over
            finishParDfs();
        } else if (dfsStack.empty()) { //tokColor == COL_BLACK
            // inactive master received black token -> start new round with white token     
            unsigned char tokCol = COL_WHITE;
            (*log) << pID << ": sending white token\n"; //DEBUG
            MPI_Send(&tokCol, 1, MPI_UNSIGNED_CHAR, (pID + 1) % pCount, MSG_TOKEN, MPI_COMM_WORLD);
            holdingToken = false;
        }
        // else: save the white token and send it when become idle


    } else { //slave        
        if (tokColor == COL_BLACK || this->color == COL_BLACK) {
            // slave is black or received black token -> resend black token immediately
            // token cannot become white, this round won't be the end
            unsigned char tokCol = COL_BLACK;
            (*log) << pID << ": sending black token\n"; //DEBUG
            MPI_Send(&tokCol, 1, MPI_UNSIGNED_CHAR, (pID + 1) % pCount, MSG_TOKEN, MPI_COMM_WORLD);
            holdingToken = false;
        } else if (dfsStack.empty()) { // tokColor == COL_WHITE && this->color == COL_WHITE
            // inactive white slave received white token -> resend white token immediately
            unsigned char tokCol = COL_WHITE;
            (*log) << pID << ": sending white token\n"; //DEBUG
            MPI_Send(&tokCol, 1, MPI_UNSIGNED_CHAR, (pID + 1) % pCount, MSG_TOKEN, MPI_COMM_WORLD);
            holdingToken = false;
        }
        // else: save the white token and send it when become idle
    }
}