#ifndef _MPPKRS_BOARD_
#define _MPPKRS_BOARD_

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define CORNER_LEN 2
#define CORNER_CELLS CORNER_LEN*CORNER_LEN

#define MAP_LEN 8

class Board {
private:
    unsigned char* map;
    int height;
    int width;
    int pins;
    int mapVagons;
    
    bool checkParams(int m, int n, int x, int q) const;
    
    /// Initially fill the board with pins
    void fill(int x, int q);
    
    void fillRandom(int count);
    
    int fillCorrect(int xCount, int bCount);
    
    bool isOnBoard(int x, int y) const;
    
    int getMapIndex(int x, int y) const;
       
    /// Place a pin on given coordinates
    bool addTo(int x, int y);
    
    /// Remove a pin pin given coordinates
    bool removeFrom(int x, int y);
   
    
    
public:
    /// Constructor with size of the board and number of pins
    Board(int m, int n, int x, int q);
    
    /// Constructor with another board
    Board(const Board& oldBoard);
    
    /// Destructor
    ~Board();
    
    /// Check if there is a pin on given coordinates
    bool isPinOn(int x, int y) const;   
    
    /// Move a pin form given coordinates in a given direction
    bool makeMove(int x, int y, int direction);
    
    bool isPossibleMove(int x, int y, int direction) const;
    
    /// Visualize in terminal
    void visualize() const;
    
    /// How many pins are there on the board
    int getPinCount() const;
    
    int getHeight() const;
    
    int getWidth() const;
};

#endif
