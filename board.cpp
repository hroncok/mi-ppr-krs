#include "board.h"
#include <ctime>
#include <cstdlib>
#include <iostream>

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
        std::cerr << "invalid arguments" << std::endl;
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

Board::Board(const Board &oldBoard) {
    this->height = oldBoard.height;
    this->width = oldBoard.width;
    this->mapVagons = oldBoard.mapVagons;
    this->pins = oldBoard.pins;

    this->map = new unsigned char[mapVagons];
    for (int i = 0; i < mapVagons; i++) {
        this->map[i] = oldBoard.map[i];
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
    for (int x = -1; x < height + 1; x++) {
        for (int y = -1; y < width + 1; y++) {
            if (!isOnBoard(x, y)) {
                std::cout << "|";
            } else if (isPinOn(x, y)) {
                std::cout << "#";
            } else std::cout << " ";
        }
        std::cout << std::endl;
    }
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
