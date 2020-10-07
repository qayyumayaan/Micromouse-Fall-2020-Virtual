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
        bool *northWall = nullptr;
        bool *southWall = nullptr;
        bool *eastWall = nullptr;
        bool *westWall = nullptr;
        Coor coords;
        int floodVal = 0;
    };

    // Personal maze map to keep track of
    Cell internalMap[16][16];

    // West/East walls
    bool xWalls[17][16];

    // South/North walls
    bool yWalls[16][17];

    // Stores latest path from the start to the center
    stack<Coor> solution;

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

    // Checks for the lowest 'flood' value among the accessible neighbors
    short findMin(vector<int>);

    // Checks for the index of the lowest 'flood' value accessible neighbor
    short findMinIndex(vector<int>,char);

    // Turns the mouse towards the desired direction (0 - West, 1 - East, 2 - South, 3 - North)
    char turnMouse(char,short);

    // Fills the walls around the center once it is reached
    void centerWalls(short,short,char);

    // Deletes the "internalMap" wall pointers
    void deleteWalls();
public:
    // Map object constructor which initializes the "internalMap"
    Map();

    // Method to be called from the Main class, has 2 modes: 0 - Initial Search; 1 - Following Searches
    void search(short);

    // Utilizes the "solution" Cell stack to traverse to the center
    void traverse();
};
