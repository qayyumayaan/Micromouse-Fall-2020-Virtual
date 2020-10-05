#include "Map.h"
#include "API.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <stack>

using namespace std;

// Map object constructor
Map::Map() {

    // Initialize map
    for(short i = 0; i < 16; i++) {
        for(short j = 0; j < 16; j++) {
            internalMap[i][j].coords.x = i;
            internalMap[i][j].coords.y = j;
            if(i==0) {
                internalMap[i][j].westWall = true;
                API::setWall(i,j,'w');
            }
            if(i==15) {
                internalMap[i][j].eastWall = true;
                API::setWall(i,j,'e');
            }
            if(j==0) {
                internalMap[i][j].southWall = true;
                API::setWall(i,j,'s');
            }
            if(j==15) {
                internalMap[i][j].northWall = true;
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

void Map::search(short mode) {

    cerr << "Starting maze search!" << endl;

    // Starting cell is on the bottom left of the maze at (0,0)
    short currX = 0;
    short currY = 0;
    char dir = 'n';

    cerr << "Pathing to center..." << endl;
    while(internalMap[currX][currY].floodVal != 0) {
        wallCheck(currX,currY,dir);

        short stepIndex = floodStep(internalMap[currX][currY],dir);

        dir = turnMouse(dir,stepIndex);
        switch (stepIndex) {
        case 0:
            currX--;
            break;
        case 1:
            currX++;
            break;
        case 2:
            currY--;
            break;
        default:
            currY++;
        }

        cerr << "Moving to (" << currX << "," << currY << ")" << endl;
        API::moveForward(1);
    }

    cerr << "Center reached!" << endl;
    short finX = currX;
    short finY = currY;

    if(mode == 0) {
        cerr << "Scouting center!" << endl;
        dir = centerFill(currX,currY,dir);
        cerr << "Center scouted!" << endl;
    }
    else {
        API::turnRight();
        API::turnRight();
        switch (dir) {
        case 'w':
            dir = 'e';
            break;
        case 'e':
            dir = 'w';
            break;
        case 's':
            dir = 'n';
            break;
        default:
            dir = 's';
        }
    }

    int maxVal = internalMap[0][0].floodVal;
    for(short i = 0; i < 16; i++) {
        for(short j = 0; j < 16; j++) {
            internalMap[i][j].floodVal = abs(maxVal - internalMap[i][j].floodVal);
            API::setText(i,j,to_string(internalMap[i][j].floodVal));
        }
    }
    for(short i = 0; i < 16; i++) {
        for(short j = 0; j < 16; j++) {
            if(i == 0 && j == 0) {
                continue;
            }
            flooder(internalMap[i][j]);
        }
    }
    solution = stack<Coor>();
    solution.push(internalMap[finX][finY].coords);

    cerr << "Pathing back to starting point..." << endl;
    while(internalMap[currX][currY].floodVal != 0) {
        wallCheck(currX,currY,dir);

        short stepIndex = floodStep(internalMap[currX][currY],dir);

        dir = turnMouse(dir,stepIndex);
        switch (stepIndex) {
        case 0:
            currX--;
            break;
        case 1:
            currX++;
            break;
        case 2:
            currY--;
            break;
        default:
            currY++;
        }

        Coor temp = solution.top();
        solution.pop();
        if(solution.size() > 1) {
            if(solution.top().x != currX || solution.top().y != currY) {
                solution.push(temp);
                solution.push(internalMap[currX][currY].coords);
            }
        }
        else {
            solution.push(temp);
            solution.push(internalMap[currX][currY].coords);
        }

        cerr << "Moving to (" << currX << "," << currY << ")" << endl;
        API::moveForward(1);
    }
    solution.pop();

    cerr << "Returned to starting point!" << endl;

    cerr << "Flipping cell values!" << endl;
    maxVal = internalMap[finX][finY].floodVal;
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 16; j++) {
            internalMap[i][j].floodVal = abs(maxVal - internalMap[i][j].floodVal);
            API::setText(i,j,to_string(internalMap[i][j].floodVal));
        }
    }
    for(short i = 0; i < 16; i++) {
        for(short j = 0; j < 16; j++) {
            if(i == finX && j == finY) {
                continue;
            }
            flooder(internalMap[i][j]);
        }
    }
    dir = turnMouse(dir,3);

    cerr << "Maze mapping complete!" << endl;
    traverse(0);
}

void Map::traverse(short mode) {

    cerr << "Starting speed run!" << endl;

    short currX = 0;
    short currY = 0;
    char dir = 'n';

    while(internalMap[currX][currY].floodVal != 0) {

        short stepIndex;
        if(mode == 0) {
            if(currX > solution.top().x) stepIndex = 0;
            else if(currX < solution.top().x) stepIndex = 1;
            else if(currY > solution.top().y) stepIndex = 2;
            else {stepIndex = 3;}
            solution.pop();
        }
        else stepIndex = findMinIndex(neighborCheck(internalMap[currX][currY]),dir);

        dir = turnMouse(dir,stepIndex);
        switch (stepIndex) {
        case 0:
            currX--;
            break;
        case 1:
            currX++;
            break;
        case 2:
            currY--;
            break;
        default:
            currY++;
        }

        cerr << "Moving to (" << currX << "," << currY << ")" << endl;
        API::moveForward(1);
    }

    cerr << "Speed run finished!" << endl;
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
            if(currX > 0) {
                API::setWall(currX-1,currY,'e');
                internalMap[currX-1][currY].eastWall = true;
            }
            internalMap[currX][currY].westWall = true;
            break;
        case 'e':
            if(currX < 15) {
                API::setWall(currX+1,currY,'w');
                internalMap[currX+1][currY].westWall = true;
            }
            internalMap[currX][currY].eastWall = true;
            break;
        case 's':
            if(currY > 0) {
                API::setWall(currX,currY-1,'n');
                internalMap[currX][currY-1].northWall = true;
            }
            internalMap[currX][currY].southWall = true;
            break;
        default:
            if(currY < 15) {
                API::setWall(currX,currY+1,'s');
                internalMap[currX][currY+1].southWall = true;
            }
            internalMap[currX][currY].northWall = true;
        }
    }

    if(API::wallLeft()) {
        API::setWall(currX,currY,left);
        switch (left) {
        case 'w':
            if(currX > 0) {
                API::setWall(currX-1,currY,'e');
                internalMap[currX-1][currY].eastWall = true;
            }
            internalMap[currX][currY].westWall = true;
            break;
        case 'e':
            if(currX < 15) {
                API::setWall(currX+1,currY,'w');
                internalMap[currX+1][currY].westWall = true;
            }
            internalMap[currX][currY].eastWall = true;
            break;
        case 's':
            if(currY > 0) {
                API::setWall(currX,currY-1,'n');
                internalMap[currX][currY-1].northWall = true;
            }
            internalMap[currX][currY].southWall = true;
            break;
        default:
            if(currY < 15) {
                API::setWall(currX,currY+1,'s');
                internalMap[currX][currY+1].southWall = true;
            }
            internalMap[currX][currY].northWall = true;
        }
    }

    if(API::wallRight()) {
        API::setWall(currX,currY,right);
        switch (right) {
        case 'w':
            if(currX > 0) {
                API::setWall(currX-1,currY,'e');
                internalMap[currX-1][currY].eastWall = true;
            }
            internalMap[currX][currY].westWall = true;
            break;
        case 'e':
            if(currX < 15) {
                API::setWall(currX+1,currY,'w');
                internalMap[currX+1][currY].westWall = true;
            }
            internalMap[currX][currY].eastWall = true;
            break;
        case 's':
            if(currY > 0) {
                API::setWall(currX,currY-1,'n');
                internalMap[currX][currY-1].northWall = true;
            }
            internalMap[currX][currY].southWall = true;
            break;
        default:
            if(currY < 15) {
                API::setWall(currX,currY+1,'s');
                internalMap[currX][currY+1].southWall = true;
            }
            internalMap[currX][currY].northWall = true;
        }
    }
}

short Map::floodStep(Cell init, char dir) {

    flooder(init);

    return findMinIndex(neighborCheck(init),dir);
}

void Map::flooder(Cell curr) {
    stack<Cell> flood;
    flood.push(curr);

    while(!flood.empty()) {
        curr = flood.top();
        flood.pop();
        int min = findMin(neighborCheck(curr));
        if(min != curr.floodVal-1) {
            internalMap[curr.coords.x][curr.coords.y].floodVal = min+1;
            API::setText(curr.coords.x,curr.coords.y,to_string(min+1));
            if(curr.coords.x > 0 && internalMap[curr.coords.x-1][curr.coords.y].floodVal != 0) flood.push(internalMap[curr.coords.x-1][curr.coords.y]);
            if(curr.coords.x < 15 && internalMap[curr.coords.x+1][curr.coords.y].floodVal != 0) flood.push(internalMap[curr.coords.x+1][curr.coords.y]);
            if(curr.coords.y > 0 && internalMap[curr.coords.x][curr.coords.y-1].floodVal != 0) flood.push(internalMap[curr.coords.x][curr.coords.y-1]);
            if(curr.coords.y < 15 && internalMap[curr.coords.x][curr.coords.y+1].floodVal != 0) flood.push(internalMap[curr.coords.x][curr.coords.y+1]);
        }
    }
}

char Map::centerFill(short currX, short currY, char dir) {
    char newDir = ' ';
    switch (currX) {
    case 7:
        switch (currY) {
        case 7:
            switch (dir) {
            case 'e':
                centerWalls(currX,currY,dir/*,{1,3,0,2}*/);
                newDir = 's';
                break;
            default:
                centerWalls(currX,currY,dir/*,{3,1,2,0}*/);
                newDir = 'w';
            }
            return newDir;
        default:
            switch (dir) {
            case 'e':
                centerWalls(currX,currY,dir/*,{1,2,0,3}*/);
                newDir = 'n';
                break;
            default:
                centerWalls(currX,currY,dir/*,{2,1,3,0}*/);
                newDir = 'w';
            }
            return newDir;
        }
        break;
    default:
        switch (currY) {
        case 7:
            switch (dir) {
            case 'w':
                centerWalls(currX,currY,dir/*,{0,3,1,2}*/);
                newDir = 's';
                break;
            default:
                centerWalls(currX,currY,dir/*,{3,0,2,1}*/);
                newDir = 'e';
            }
            return newDir;
        default:
            switch (dir) {
            case 'w':
                centerWalls(currX,currY,dir/*,{0,2,1,3}*/);
                newDir = 'n';
                break;
            default:
                centerWalls(currX,currY,dir/*,{2,0,3,1}*/);
                newDir = 'e';
            }
            return newDir;
        }
    }
}

void Map::centerWalls(short currX, short currY, char dir) {
//    vector<short> steps;
//    for(short i = 0; i < 4; i++) {
//        wallCheck(currX,currY,dir);
//        switch (steps[i]) {
//        case 0:
//            currX--;
//            break;
//        case 1:
//            currX++;
//            break;
//        case 2:
//            currY--;
//            break;
//        default:
//            currY++;
//        }
//        dir = turnMouse(dir,steps[i]);
//        API::moveForward();
//    }
    switch (dir) {
    case 'w':
        if(currX == 8 && currY == 8) {
            internalMap[8][8].northWall = true;
            internalMap[8][9].southWall = true;
            internalMap[7][8].northWall = true;
            internalMap[7][9].southWall = true;
            internalMap[7][8].westWall = true;
            internalMap[7][6].eastWall = true;
            internalMap[7][7].southWall = true;
            internalMap[7][6].northWall = true;
            internalMap[8][7].southWall = true;
            internalMap[8][6].northWall = true;
        }
    case 'e':
    case 's':
    default:
        break;
    }
}

vector<int> Map::neighborCheck(Cell currCell) {
    vector<int> neighbors = {-1,-1,-1,-1};
    if(currCell.coords.x > 0 && currCell.westWall == false) {
        neighbors[0] = internalMap[currCell.coords.x-1][currCell.coords.y].floodVal;
    }
    if(currCell.coords.x < 15 && currCell.eastWall == false) {
        neighbors[1] = internalMap[currCell.coords.x+1][currCell.coords.y].floodVal;
    }
    if(currCell.coords.y > 0 && currCell.southWall == false) {
        neighbors[2] = internalMap[currCell.coords.x][currCell.coords.y-1].floodVal;
    }
    if(currCell.coords.y < 15 && currCell.northWall == false) {
        neighbors[3] = internalMap[currCell.coords.x][currCell.coords.y+1].floodVal;
    }
    return neighbors;
}

short Map::findMin(vector<int> neighbors) {
    short check = 0;
    int min = -1;

    while(check < 4) {
        if(neighbors[check] == -1) {
            check++;
            continue;
        }
        if(min == -1) {
            min = neighbors[check];
            check++;
            continue;
        }
        if(neighbors[check] < min) {
            min = neighbors[check];
        }
        check++;
    }
    return min;
}

short Map::findMinIndex(vector<int> neighbors, char dir) {
    short check = 0;
    int min = -1;
    short stepIndex = -1;

    while(check < 4) {
        if(neighbors[check] == -1) {
            check++;
            continue;
        }
        if(min == -1) {
            min = neighbors[check];
            stepIndex = check;
            check++;
            continue;
        }
        if(neighbors[check] == min) {
            switch (dir) {
            case 'w':
                if(check == 0) stepIndex = check;
                break;
            case 'e':
                if(check == 1) stepIndex = check;
                break;
            case 's':
                if(check == 2) stepIndex = check;
                break;
            default:
                if(check == 3) stepIndex = check;
            }
        }
        if(neighbors[check] < min) {
            min = neighbors[check];
            stepIndex = check;
        }
        check++;
    }
    return stepIndex;
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
            return 'e';
        case 2:
            API::turnLeft();
            return 's';
        default:
            API::turnRight();
            return 'n';
        }
    case 'e':
        switch (next) {
        case 0:
            API::turnRight();
            API::turnRight();
            return 'w';
        case 1:
            return 'e';
        case 2:
            API::turnRight();
            return 's';
        default:
            API::turnLeft();
            return 'n';
        }
    case 's':
        switch (next) {
        case 0:
            API::turnRight();
            return 'w';
        case 1:
            API::turnLeft();
            return 'e';
        case 2:
            return 's';
        default:
            API::turnRight();
            API::turnRight();
            return 'n';
        }
    default:
        switch (next) {
        case 0:
            API::turnLeft();
            return 'w';
        case 1:
            API::turnRight();
            return 'e';
        case 2:
            API::turnRight();
            API::turnRight();
            return 's';
        default:
            return 'n';
        }
    }
}
