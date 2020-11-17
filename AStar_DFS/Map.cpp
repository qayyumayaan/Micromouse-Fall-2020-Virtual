#include "Map.h"
#include "API.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <stack>

using namespace std;

// Map object constructor
Map::Map() {

    // Initialize map and walls
    for(short i = 0; i < 16; i++) {
        for(short j = 0; j < 16; j++) {
            internalMap[i][j].coords.x = i;
            internalMap[i][j].coords.y = j;
            internalMap[i][j].westWall = &xWalls[i][j];
            internalMap[i][j].eastWall = &xWalls[i+1][j];
            internalMap[i][j].southWall = &yWalls[i][j];
            internalMap[i][j].northWall = &yWalls[i][j+1];
            if(i==0) {
                *internalMap[i][j].westWall = true;
                API::setWall(i,j,'w');
            }
            if(i==15) {
                *internalMap[i][j].eastWall = true;
                API::setWall(i,j,'e');
            }
            if(j==0) {
                *internalMap[i][j].southWall = true;
                API::setWall(i,j,'s');
            }
            if(j==15) {
                *internalMap[i][j].northWall = true;
                API::setWall(i,j,'n');
            }
            if(i < 8 && j < 8) internalMap[i][j].floodVal = 7-i + 7-j;
            else if(i < 8 && j >= 8) internalMap[i][j].floodVal = 7-i + j-8;
            else if(i >= 8 && j >= 8) internalMap[i][j].floodVal = i-8 + j-8;
            else internalMap[i][j].floodVal = i-8 + 7-j;
            API::setText(i,j,to_string(internalMap[i][j].floodVal));
        }
    }
    cerr << "Maze values initialized!" << endl;
}

// Input code for DFS in this method
void Map::dfs() {
    internalMap[0][0].visited = true;
    wallCheck(0,0,'n');
}

// Input code for A* in this method
void Map::astar() {

}

char Map::turnMouse(char dir, short next) {
    switch (dir) {
    case 'w':
        switch (next) {
        case 0:
            return 'w';
        case 1:
            API::turnRight();
            API::turnRight();
            totalTurns+=2;
            return 'e';
        case 2:
            API::turnLeft();
            totalTurns++;
            return 's';
        default:
            API::turnRight();
            totalTurns++;
            return 'n';
        }
    case 'e':
        switch (next) {
        case 0:
            API::turnRight();
            API::turnRight();
            totalTurns+=2;
            return 'w';
        case 1:
            return 'e';
        case 2:
            API::turnRight();
            totalTurns++;
            return 's';
        default:
            API::turnLeft();
            totalTurns++;
            return 'n';
        }
    case 's':
        switch (next) {
        case 0:
            API::turnRight();
            totalTurns++;
            return 'w';
        case 1:
            API::turnLeft();
            totalTurns++;
            return 'e';
        case 2:
            return 's';
        default:
            API::turnRight();
            API::turnRight();
            totalTurns+=2;
            return 'n';
        }
    default:
        switch (next) {
        case 0:
            API::turnLeft();
            totalTurns++;
            return 'w';
        case 1:
            API::turnRight();
            totalTurns++;
            return 'e';
        case 2:
            API::turnRight();
            API::turnRight();
            totalTurns+=2;
            return 's';
        default:
            return 'n';
        }
    }
}

void Map::wallCheck(short currX, short currY, char dir) {
    char front,left,right = ' ';

    switch (dir) {
    case 'n':
        front = 'n';
        left = 'w';
        right = 'e';
        break;
    case 's':
        front = 's';
        left = 'e';
        right = 'w';
        break;
    case 'w':
        front = 'w';
        left = 's';
        right = 'n';
        break;
    default:
        front = 'e';
        left = 'n';
        right = 's';
    }

    if(API::wallFront()) {
        API::setWall(currX,currY,front);
        switch (front) {
        case 'w':
            *internalMap[currX][currY].westWall = true;
            break;
        case 'e':
            *internalMap[currX][currY].eastWall = true;
            break;
        case 's':
            *internalMap[currX][currY].southWall = true;
            break;
        default:
            *internalMap[currX][currY].northWall = true;
        }
    }

    if(API::wallLeft()) {
        API::setWall(currX,currY,left);
        switch (left) {
        case 'w':
            *internalMap[currX][currY].westWall = true;
            break;
        case 'e':
            *internalMap[currX][currY].eastWall = true;
            break;
        case 's':
            *internalMap[currX][currY].southWall = true;
            break;
        default:
            *internalMap[currX][currY].northWall = true;
        }
    }

    if(API::wallRight()) {
        API::setWall(currX,currY,right);
        switch (right) {
        case 'w':
            *internalMap[currX][currY].westWall = true;
            break;
        case 'e':
            *internalMap[currX][currY].eastWall = true;
            break;
        case 's':
            *internalMap[currX][currY].southWall = true;
            break;
        default:
            *internalMap[currX][currY].northWall = true;
        }
    }
}
