#pragma once

#include<string>
#include<vector>
#include<stack>

using namespace std;

class Map
{
private:
    // Struct for maze cells
    struct Coor {
        short x = 0;
        short y = 0;
    };

    struct Cell {
        bool *northWall;
        bool *southWall;
        bool *eastWall;
        bool *westWall;
        Coor coords;
        int floodVal = 0;
    };

    // Personal maze map to keep track of
    Cell internalMap[16][16];

    bool xWalls[17][16];

    bool yWalls[16][17];

    stack<Coor> solution;

    // Checks and updates walls on the given cell and its neighbors based on the direction of the bot
    bool solutionCheck(stack<Coor>);
    short floodStep(Cell,char);
    void flooder(Cell);
    vector<int> neighborCheck(Cell);
    void wallCheck(short,short,char);
    short findMin(vector<int>);
    short findMinIndex(vector<int>,char);
    char turnMouse(char,short);
    char centerFill(short,short,char);
    void centerWalls(short,short,char);
    void deleteWalls();
public:
    // Map object constructor which initializes the internalMap
    Map();

    // Modified Floodfill to be called from the Main class
    void search(short);
    void traverse(short);
};
