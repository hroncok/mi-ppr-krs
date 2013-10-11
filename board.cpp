#include "board.h"
#include <stdlib.h>
#include <stdio.h>

Board::Board(int m_, int n_, int x, int q) {
	m = m_ + 2*CORNER;
	n = n_ + 2*CORNER;
	// Prepare the bitmap
	int fields = m_*n_ + 2*CORNER*m + 2*CORNER*n;
	if (q > fields-m_-n_)
		perror("q should be smaller than total fields-m_-n_!");
	ints = ((fields-1)/CAPACITY)+1; // gets 1 for CAPACITY, but 2 for CAPACITY+1
	map = new int [ints];
	for (int i = 0; i < ints; i++) {
		map[i] = 0;
	}
	fill(x,q);
}

void Board::fill(int x, int q) {
	// Fill it with x "random" pins
	for (int i = 0; i < x; i++) {
		int vagon = rand() % ints;
		int seat = rand() % CAPACITY;
		int mask = 1 << seat;
		if ((map[vagon] & mask) != 0) {
			i--;
			continue;
		}
		map[vagon] |= mask;
	}
	
	// Perform backmoves
	for (int i = 0; i <= q-x; i++) {
		bool pin = false;
		for (int x_ = 0; x_ < m; x_++) {
			for (int y_ = 0; y_ < n; y_++) {
				if (!is_on(x_,y_)) continue;
				for (int way = 0; way < 4; way++) {
					switch (way) {
						case NORTH:
							if ((x_ > 1) && (is_on(x_-1,y_)) && (!is_on(x_-2,y_))) {
								remove_from(x_,y_);
								remove_from(x_-1,y_);
								add_to(x_-2,y_);
								pin = true;
							}
							break;
						case EAST:
							if ((y_ <= n-2) && (is_on(x_,y_+1)) && (!is_on(x_,y_+2))) {
								remove_from(x_,y_);
								remove_from(x_,y_+1);
								add_to(x_,y_+2);
								pin = true;
							}
							break;
						case SOUTH:
							if ((x_ <= m-2) && (is_on(x_+1,y_)) && (!is_on(x_+2,y_))) {
								remove_from(x_,y_);
								remove_from(x_+1,y_);
								add_to(x_+2,y_);
								pin = true;
							}
							break;
						default:
							if ((y_ > 1) && (is_on(x_,y_-1)) && (!is_on(x_,y_-2))) {
								remove_from(x_,y_);
								remove_from(x_,y_-1);
								add_to(x_,y_-2);
								pin = true;
							}
							break;
					}
					if (pin) break;
				}
				if (pin) break;
			}
			if (pin) break;
		}
	}
}

Board::~Board() {
	delete [] map;
}

bool Board::in_corner(int x, int y) {
	return ((x < CORNER && y < CORNER) ||
	   (x < CORNER && y > n-CORNER) ||
	   (x > m-CORNER && y < CORNER) ||
	   (x > m-CORNER && y > n-CORNER) ||
	   (x > m) ||
	   (y > n) ||
	   (x < 0) ||
	   (y < 0));
}

bool Board::is_on(int x, int y) {
	if (in_corner(x,y)) return false;
	int test = num_from_cords(x,y);
	int vagon = test / CAPACITY;
	int seat = test % CAPACITY;
	int mask = 1 << seat;
	if ((map[vagon] & mask) == 0) {
		return false;
	}
	return true;
}

bool Board::add_to(int x, int y) {
	if (in_corner(x,y)) return false;
	int add = num_from_cords(x,y);
	int vagon = add / CAPACITY;
	int seat = add % CAPACITY;
	int mask = 1 << seat;
	if ((map[vagon] & mask) != 0) {
		return false;
	}
	map[vagon] |= mask;
	return true;
}

bool Board::remove_from(int x, int y) {
	if (in_corner(x,y)) return false;
	int add = num_from_cords(x,y);
	int vagon = add / CAPACITY;
	int seat = add % CAPACITY;
	int mask = 1 << seat;
	if ((map[vagon] & mask) == 0) {
		return false;
	}
	map[vagon] &= (~mask);
	return true;
}

int Board::num_from_cords(int x, int y) {
	int num = y*m + x;
	num -= 2*CORNERS;
	if (y == 0) num += 3*CORNER;
	if (y == 1) num += CORNER;
	if (y == n-1) num -= 3*CORNER;
	if (y == n-2) num -= CORNER;
	return num;
}

Board::Board(const Board &b) {
	ints = b.ints;
	map = new int[ints];
	for (int i = 0; i < ints; i++) {
		map[i] = b.map[i];
	}
	m = b.m;
	n = b.n;
}


bool Board::move(int x, int y, int direction) {
	if (!is_on(x,y)) return false;
	switch (direction) {
		case NORTH:
			if (x < 2) return false;
			if (!is_on(x-1,y)) return false;
			if (is_on(x-2,y)) return false;
			remove_from(x,y);
			remove_from(x-1,y);
			add_to(x-2,y);
			return true;
		case EAST:
			if (y > n-3) return false;
			if (!is_on(x,y+1)) return false;
			if (is_on(x,y+2)) return false;
			remove_from(x,y);
			remove_from(x,y+1);
			add_to(x,y+2);
			return true;
		case SOUTH:
			if (x > m-3) return false;
			if (!is_on(x+1,y)) return false;
			if (is_on(x+2,y)) return false;
			remove_from(x,y);
			remove_from(x+1,y);
			add_to(x+2,y);
			return true;
		default:
			if (y < 2) return false;
			if (!is_on(x,y-1)) return false;
			if (is_on(x,y-2)) return false;
			remove_from(x,y);
			remove_from(x,y-1);
			add_to(x,y-2);
			return true;
	}
}

int Board::pins() {
	int counter = 0;
	for (int x = 0; x < m; x++) {
		for (int y = 0; y < n; y++) {
			if (is_on(x,y)) counter++;
		}
	}
	return counter;
}

