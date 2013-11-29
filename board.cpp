#include "board.h"
#include "logger.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>
#include <mpi.h>

bool Board::checkParams(int m, int n, int x, int q) const {
    if ((m < 3) || (m < n)) return false;

    int M = (m * n)+(4 * m)+(4 * n);
    if (x == 0) {
        if ((q <= M) && (q > 0)) return true;
        else return false;
    }

    if ((q > M - m - n) || (q < 1)) return false;
    if ((x >= q) || (x < 1)) return false;

    return true;
}

void Board::fill(int xCount, int qCount) {
    srand(time(NULL));
    if (xCount == 0) {
        fillRandom(qCount);
        this->pins = qCount;
    } else {
        this->pins = fillCorrect(xCount, qCount - xCount + 1);
    }
}

int Board::fillCorrect(int xCount, int bCount) {
    int totalPins = xCount + bCount;

    // xCount random pins
    for (int i = 0; i < xCount; i++) {
        int x = rand() % height;
        int y = rand() % width;

        if ((!isOnBoard(x, y)) || (isPinOn(x, y)) || (isPinOn(x + 1, y)) ||
                (isPinOn(x - 1, y)) || (isPinOn(x, y + 1)) || (isPinOn(x, y - 1))) {
            i--;
        } else {
            addTo(x, y);
        }
    }

    //bCount back moves
    bool inserted = true;
    while (inserted) {
        inserted = false;
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                if ((isPinOn(x, y)) && (isOnBoard(x, y + 2)) && (!isPinOn(x, y + 1)) && (!isPinOn(x, y + 2))) {
                    removeFrom(x, y);
                    addTo(x, y + 1);
                    addTo(x, y + 2);
                    inserted = true;
                    bCount--;
                    if (bCount == 0) return totalPins;
                }

                if ((isPinOn(x, y + 2)) && (isOnBoard(x, y)) && (!isPinOn(x, y + 1)) && (!isPinOn(x, y))) {
                    removeFrom(x, y + 2);
                    addTo(x, y + 1);
                    addTo(x, y);
                    inserted = true;
                    bCount--;
                    if (bCount == 0) return totalPins;
                }
            }
        }

        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                if ((isPinOn(x, y)) && (isOnBoard(x + 2, y)) && (!isPinOn(x + 1, y)) && (!isPinOn(x + 2, y))) {
                    removeFrom(x, y);
                    addTo(x + 1, y);
                    addTo(x + 2, y);
                    inserted = true;
                    bCount--;
                    if (bCount == 0) return totalPins;
                }

                if ((isPinOn(x + 2, y)) && (isOnBoard(x, y)) && (!isPinOn(x + 1, y)) && (!isPinOn(x, y))) {
                    removeFrom(x + 2, y);
                    addTo(x + 1, y);
                    addTo(x, y);
                    inserted = true;
                    bCount--;
                    if (bCount == 0) return totalPins;
                }
            }
        }
    }
    return totalPins - bCount;
}

void Board::fillRandom(int count) {
    for (int i = 0; i < count; i++) {
        int x = rand() % height;
        int y = rand() % width;

        if ((!isOnBoard(x, y)) || (isPinOn(x, y))) {
            i--;
        } else {
            addTo(x, y);
        }
    }
}

bool Board::isOnBoard(int x, int y) const {
    if ((x >= height) || (y >= width) || (x < 0) || (y < 0)) {
        return false;
    }

    bool inCorner = (((x < CORNER_LEN) && (y < CORNER_LEN)) ||
            ((x < CORNER_LEN) && (y >= width - CORNER_LEN)) ||
            ((x >= height - CORNER_LEN) && (y < CORNER_LEN)) ||
            ((x >= height - CORNER_LEN) && (y >= width - CORNER_LEN)));

    return (!inCorner);
}

int Board::getMapIndex(int x, int y) const {
    if (!isOnBoard(x, y)) return -1;


    int seqNum = (x * width) + y;
    // works only with CORNER_LEN == 2
    if (x >= 0) seqNum -= CORNER_LEN;
    if (x >= 1) seqNum -= (2 * CORNER_LEN);
    if (x >= 2) seqNum -= CORNER_LEN;

    if (x >= height - 2) seqNum -= CORNER_LEN;
    if (x >= height - 1) seqNum -= (2 * CORNER_LEN);

    return seqNum;
}

bool Board::addTo(int x, int y) {
    if (isPinOn(x, y)) return false;

    int seqIndex = getMapIndex(x, y);
    int vagon = seqIndex / MAP_LEN;
    int seat = seqIndex % MAP_LEN;
    unsigned char mask = 1 << seat;

    map[vagon] |= mask;
    return true;
}

bool Board::removeFrom(int x, int y) {
    if (!isPinOn(x, y)) return false;

    int seqIndex = getMapIndex(x, y);
    int vagon = seqIndex / MAP_LEN;
    int seat = seqIndex % MAP_LEN;
    unsigned char mask = 1 << seat;

    map[vagon] &= (~mask);
    return true;
}

Board::Board(int m, int n, int x, int q) {
    if (!checkParams(m, n, x, q)) {        
        (*Logger::getInstance()) << "invalid arguments\n";   
        MPI_Finalize();
        exit(1);
    }

    this->height = m + 2 * CORNER_LEN;
    this->width = n + 2 * CORNER_LEN;

    int cells = (height * width) - (CORNER_CELLS * 4);
    this->mapVagons = cells / MAP_LEN;
    if (cells % MAP_LEN != 0) mapVagons++;
    this->map = new unsigned char[mapVagons];

    for (int i = 0; i < mapVagons; i++) {
        map[i] = 0;
    }

    fill(x, q);
}

Board::Board(int m) {
    this->height = 5 + 2 * CORNER_LEN;
    this->width = 5 + 2 * CORNER_LEN;

    int cells = (height * width) - (CORNER_CELLS * 4);
    this->mapVagons = cells / MAP_LEN;
    if (cells % MAP_LEN != 0) mapVagons++;
    this->map = new unsigned char[mapVagons];

    if (m == 1) {
        map[0] = 44;
        map[1] = 144;
        map[2] = 6;
        map[3] = 0;
        map[4] = 160;
        map[5] = 154;
        map[6] = 90;
        map[7] = 0;
        map[8] = 0;
    } else if (m == 2) {
        map[0] = 0;
        map[1] = 192;
        map[2] = 128;
        map[3] = 1;
        map[4] = 26;
        map[5] = 128;
        map[6] = 90;
        map[7] = 13;
        map[8] = 0;
    } else {
        map[0] = 90;
        map[1] = 3;
        map[2] = 0;
        map[3] = 0;
        map[4] = 128;
        map[5] = 26;
        map[6] = 106;
        map[7] = 192;
        map[8] = 0;
    }
}

Board::Board(const Board &oldBoard) {
    this->height = oldBoard.height;
    this->width = oldBoard.width;
    this->pins = oldBoard.pins;
    this->mapVagons = oldBoard.mapVagons;

    this->map = new unsigned char[mapVagons];
    for (int i = 0; i < mapVagons; i++) {
        this->map[i] = oldBoard.map[i];
    }
}

Board::Board(const unsigned char* serMap) {
    this->height = serMap[0];
    this->width = serMap[1];
    this->pins = serMap[2];
    this->mapVagons = serMap[3];

    this->map = new unsigned char[this->mapVagons];
    for (int i = 0; i < this->mapVagons; i++) {
        this->map[i] = serMap[i + 4];
    }
}

Board::~Board() {
    delete[] map;
}

bool Board::isPinOn(int x, int y) const {
    if (!isOnBoard(x, y)) return false;

    int seqIndex = getMapIndex(x, y);
    int vagon = seqIndex / MAP_LEN;
    int seat = seqIndex % MAP_LEN;
    unsigned char mask = 1 << seat;

    if ((map[vagon] & mask) != 0) return true;
    else return false;
}

bool Board::makeMove(int x, int y, int direction) {
    if (!isPossibleMove(x, y, direction)) return false;

    switch (direction) {
        case NORTH:
            removeFrom(x, y);
            removeFrom(x - 1, y);
            addTo(x - 2, y);
            this->pins--;
            return true;
        case EAST:
            removeFrom(x, y);
            removeFrom(x, y + 1);
            addTo(x, y + 2);
            this->pins--;
            return true;
        case SOUTH:
            removeFrom(x, y);
            removeFrom(x + 1, y);
            addTo(x + 2, y);
            this->pins--;
            return true;
        case WEST:
            removeFrom(x, y);
            removeFrom(x, y - 1);
            addTo(x, y - 2);
            this->pins--;
            return true;
        default:
            return false;
    }
}

bool Board::isPossibleMove(int x, int y, int direction) const {
    if (!isPinOn(x, y)) return false;

    switch (direction) {
        case NORTH:
            if (!isOnBoard(x - 2, y)) return false;
            if ((!isPinOn(x - 1, y)) || (isPinOn(x - 2, y))) return false;
            return true;
        case EAST:
            if (!isOnBoard(x, y + 2)) return false;
            if ((!isPinOn(x, y + 1)) || (isPinOn(x, y + 2))) return false;
            return true;
        case SOUTH:
            if (!isOnBoard(x + 2, y)) return false;
            if ((!isPinOn(x + 1, y)) || (isPinOn(x + 2, y))) return false;
            return true;
        case WEST:
            if (!isOnBoard(x, y - 2)) return false;
            if ((!isPinOn(x, y - 1)) || (isPinOn(x, y - 2))) return false;
            return true;
        default:
            return false;
    }
}

void Board::visualize() const {
    std::string buf = "";

    for (int x = -1; x < height + 1; x++) {
        for (int y = -1; y < width + 1; y++) {
            if (!isOnBoard(x, y)) {
                buf += "|";
            } else if (isPinOn(x, y)) {
                buf += "#";
            } else buf += " ";
        }
        buf+="\n";
    }
    
    (*Logger::getInstance()) << buf;    
}

int Board::getPinCount() const {
    return this->pins;
}

int Board::getHeight() const {
    return this->height;
}

int Board::getWidth() const {
    return this->width;
}

unsigned char* Board::getSerializedBoard() const {
    unsigned char* serMap = new unsigned char[this->mapVagons + 4];

    serMap[0] = this->height;
    serMap[1] = this->width;
    serMap[2] = this->pins;
    serMap[3] = this->mapVagons;

    for (int i = 0; i < this->mapVagons; i++) {
        serMap[i + 4 ] = this->map[i];
    }

    return serMap;
}
