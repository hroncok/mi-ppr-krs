#ifndef _MPPKRS_BOARD_
#define _MPPKRS_BOARD_

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define CORNER_LEN 2
#define CORNER_CELLS CORNER_LEN*CORNER_LEN

#define MAP_LEN 8

///Class represents current playboard
class Board {
private:
    unsigned char* map; //pointer to bitmaps
    int height;
    int width;
    int pins; //current number of remaining pins
    int mapVagons; //number of bitmap-vagons
    
    /// True if all parameters are correct
    bool checkParams(int m, int n, int x, int q) const;
    
    /// Initially fill the board with pins
    void fill(int x, int q);
    
    /// Place count pins randomly on the board
    void fillRandom(int count);
    
    /// Place xCount pins randomly, then make bCount backmoves
    int fillCorrect(int xCount, int bCount);
    
    /// True if there is a pin at [x,y]
    bool isOnBoard(int x, int y) const;
    
    /// Get sequential number of [x.y]
    int getMapIndex(int x, int y) const;
       
    /// Place a pin on given coordinates
    bool addTo(int x, int y);
    
    /// Remove a pin pin given coordinates
    bool removeFrom(int x, int y);
   
    
    
public:
    /// Constructor with size of the board and number of pins
    Board(int m, int n, int x, int q);

    /// Constructor with preset tests
    Board(int m);
    
    /// Constructor with another board
    Board(const Board& oldBoard);
    
    /// Constructor with serialized Board
    Board(const unsigned char* serMap);
    
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
    
    /// Get height of the board
    int getHeight() const;
    
    /// Get width of the board
    int getWidth() const;
    
    /// Get serialized board (byte array)
    unsigned char* getSerializedBoard() const;
};

#endif
