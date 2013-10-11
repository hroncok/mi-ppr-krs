#ifndef _MPPKRS_BOARD_
#define _MPPKRS_BOARD_

#define CAPACITY 32

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define CORNER 2
#define CORNERS CORNER*CORNER

#include <string>

class Board {
	int * map;
	int ints;
	int m, n;
	int numsize;
	/// Initially fill the broad with pins
	void fill(int x, int q);
	/// Get a single number form coordinates
	int num_from_cords(int x, int y);
	/// Place a pin on given coordinates
	bool add_to(int x, int y);
	/// Remove a pin pin given coordinates
	bool remove_from(int x, int y);
	/// Is in corner?
	bool in_corner(int x, int y);
public:
	/// Constructor with size of the board and number of pins
	Board(int m_, int n_, int x, int q);
	/// Constructor with another board
	Board(const Board &b);
	/// Destructor
	~Board();
	/// Check is there is a pin on given coordinates
	bool is_on(int x, int y);
	/// Move a pin form given coordinates in a given direction
	std::string move(int x, int y, int direction);
	/// How many pins are there on the board
	int pins();
	/// Visualize in terminal
	void visualize();
};

#endif
