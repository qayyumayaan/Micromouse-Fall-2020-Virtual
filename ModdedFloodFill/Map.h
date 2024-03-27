#pragma once

#include<string>
#include<vector>
#include<stack>

using namespace std;

class Map
{
private:
    // Struct for coordinates
    struct Coor {
        short x = 0;
        short y = 0;
    };

    //Struct for maze cells
    struct Cell {
        bool visited = false;
        bool *northWall = 0;
        bool *southWall = 0;
        bool *eastWall = 0;
        bool *westWall = 0;
        Coor coords;
        int floodVal = 0;
    };

    // Personal maze map to keep track of
    Cell internalMap[16][16];

    // West/East walls
    bool xWalls[17][16] = {{false}};

    // South/North walls
    bool yWalls[16][17] = {{false}};

    // Maze cell discovered count
    int cellVisitCount = 0;

    // Total turns
    int totalTurns = 0;

    // Total distance
    double totalDist = 0;

    // Best turns
    int bestTurns = 0;

    // Best distance
    double bestDist = 0;

    // Stores latest path from the start to the center
    stack<Coor> solution;

    // Function for moving forward multiple cells at a time across previously visted cells
    short lookAhead(short,short,char);

    // Function to check whether path back to start has already been traversed over, returns resulting score if path has been traversed over and -1 otherwise
    int futurePathCheck(short,short,char);

    // Checks the solution stack for 'flood' value consistency
    bool solutionCheck(stack<Coor>);

    // Method utilized when searching through the array, depends on the 'flooder' method
    short floodStep(Cell,char);

    // 'Floods' the given cell and its neighboring cells when needed
    void flooder(Cell);

    // Checks and updates walls on the given cell and its neighbors based on the direction of the bot
    vector<int> neighborCheck(Cell);

    // Checks for walls around the bot
    void wallCheck(short,short,char);

    void setWallIf(bool condition, short currX, short currY, char direction);

    // Checks for the lowest 'flood' value among the accessible neighbors
    short findMin(vector<int>);

    // Checks for the index of the lowest 'flood' value accessible neighbor
    short findMinIndex(vector<int>,char);

    // Turns the mouse towards the desired direction (0 - West, 1 - East, 2 - South, 3 - North)
    char turnMouse(char,short);

    char turnWest(short next);

    char turnEast(short next);

    char turnSouth(short next);

    char turnNorth(short next);

    // Fills the walls around the center once it is reached
    void centerWalls(short,short,char);
    void setWallAndInternalMap(short x, short y, char direction);

public:
    // Map object constructor which initializes the "internalMap"
    Map();

    // Method to be called from the Main class, has 2 modes: 0 - Initial Search; 1 - Following Searches
    void search(bool);

    // Utilizes the "solution" Cell stack to traverse to the center
    void traverse();
};
