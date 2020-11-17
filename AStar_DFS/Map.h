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
        vector<Coor> adj;
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

    // Total turns
    int totalTurns = 0;

    // Total distance
    int totalDist = 0;

    // Best turns
    int bestTurns = 0;

    // Best distance
    int bestDist = 0;

    // Checks for walls around the bot
    void wallCheck(short,short,char);

    // Turns the mouse towards the desired direction (0 - West, 1 - East, 2 - South, 3 - North)
    char turnMouse(char,short);

public:
    // Map object constructor which initializes the "internalMap"
    Map();

    // DFS traversal method to map maze
    void dfs();

    // A* search algorithm
    void astar();
};
