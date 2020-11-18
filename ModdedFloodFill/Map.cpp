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

void Map::search(bool centerCheck) {

    // Starting cell is on the bottom left of the maze at (0,0) with default direction being North
    short currX = 0;
    short currY = 0;
    char dir = 'n';

    if(centerCheck) cerr << "Starting next run!" << endl;
    else cerr << "Starting maze search!" << endl;
    cerr << "Pathing to center..." << endl;

    bool pathStatus = true; // This variable checks whether any flooding occurs in the run

    int currTurns = 0;
    int currDist = 0;

    while(internalMap[currX][currY].floodVal != 0) {

        if(API::wasReset()) {
            cerr << "Resetting!" << endl;
            return;
        }

        wallCheck(currX,currY,dir);

        internalMap[currX][currY].visited = true;

        int tempVal = internalMap[currX][currY].floodVal;

        short stepIndex = floodStep(internalMap[currX][currY],dir); // Stores neighbor to move toward after calling 'flooder'

        if(tempVal != internalMap[currX][currY].floodVal) pathStatus = false;

        char prevDir = dir;
        dir = turnMouse(dir,stepIndex);
        short moveDist = 0;

        switch (dir) {
        case 'w':
            if (prevDir == 'e') {
                currTurns+=2;
            }
            else if (prevDir == 'n' || prevDir == 's') {
                currTurns++;
            }
            moveDist = lookAhead(currX-1,currY,dir);
            currX-=moveDist;
            break;
        case 'e':
            if (prevDir == 'w') {
                currTurns+=2;
            }
            else if (prevDir == 'n' || prevDir == 's') {
                currTurns++;
            }
            moveDist = lookAhead(currX+1,currY,dir);
            currX+=moveDist;
            break;
        case 's':
            if (prevDir == 'n') {
                currTurns+=2;
            }
            else if (prevDir == 'w' || prevDir == 'e') {
                currTurns++;
            }
            moveDist = lookAhead(currX,currY-1,dir);
            currY-=moveDist;
            break;
        default:
            if (prevDir == 's') {
                currTurns+=2;
            }
            else if (prevDir == 'w' || prevDir == 'e') {
                currTurns++;
            }
            moveDist = lookAhead(currX,currY+1,dir);
            currY+=moveDist;
        }

        cerr << "Moving to (" << currX << "," << currY << ")" << endl;
        API::moveForward(moveDist);
        totalDist++;
        currDist++;
    }

    if (bestDist == 0) {
        bestDist = currDist;
        bestTurns = currTurns;
    }
    if (currDist < bestDist) {
        bestDist = currDist;
    }
    if (currTurns < bestTurns) {
        bestTurns = currTurns;
    }

    cerr << "Center reached!" << endl;

    // No need to continue run if no flooding occured on the path
    if(pathStatus) {
        cerr << "Run complete!" << endl;
        cerr << "Speedrun achieved!" << endl;
        return;
    }

    // If the center has not been previously reached, set its walls
    if(!centerCheck) {
        cerr << "Setting center walls!" << endl;
        centerWalls(currX,currY,dir);
        cerr << "Center walls set!" << endl;
    }

    // Stores the coordinates of the cell accessed at the center
    short finX = currX;
    short finY = currY;

    cerr << "Flipping cell values!" << endl;
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

    double currentScore = bestDist + bestTurns + 0.1*(totalDist + totalTurns);

    double unoptimalRatio = (totalTurns*1.0)/totalDist;
    double optimalRatio = (currTurns*1.0)/currDist;

    int estimateBestDist = internalMap[currX][currY].floodVal;
    double unoptimalTurns = estimateBestDist*unoptimalRatio;
    double optimalTurns = estimateBestDist*optimalRatio;

    double estimateTotal = currDist + currTurns + 6*(estimateBestDist + unoptimalTurns);
    double estimateBestScore = estimateBestDist + optimalTurns + 0.1*(estimateTotal);

    cerr << (unoptimalRatio) << endl;
    cerr << (estimateBestScore/currentScore) << endl;

    if ((estimateBestScore/currentScore) > 0.9) {
        cerr << "Estimated future score is unoptimal!" << endl;
        return;
    }

    // This stack stores the path to follow back to the center
    // (Only followed back if it is continuous)
    solution = stack<Coor>();
    solution.push(internalMap[finX][finY].coords);

    cerr << "Pathing back to starting point..." << endl;

    while(internalMap[currX][currY].floodVal != 0) {

        if(API::wasReset()) {
            cerr << "Resetting!" << endl;
            return;
        }

        wallCheck(currX,currY,dir);

        internalMap[currX][currY].visited = true;

        short stepIndex = floodStep(internalMap[currX][currY],dir);

        dir = turnMouse(dir,stepIndex);
        short moveDist = 0;
        short checkX = currX; short checkY = currY;

        switch (stepIndex) {
        case 0:
            moveDist = lookAhead(currX-1,currY,dir);
            currX-=moveDist;
            checkX--;
            break;
        case 1:
            moveDist = lookAhead(currX+1,currY,dir);
            currX+=moveDist;
            checkX++;
            break;
        case 2:
            moveDist = lookAhead(currX,currY-1,dir);
            currY-=moveDist;
            checkY--;
            break;
        default:
            moveDist = lookAhead(currX,currY+1,dir);
            currY+=moveDist;
            checkY++;
        }

        // This part is to take out any cells that the mouse backtracks from
        Coor temp = solution.top();
        solution.pop();
        if(solution.size() > 1) {
            if(solution.top().x != checkX || solution.top().y != checkY) {
                solution.push(temp);
                solution.push(internalMap[currX][currY].coords);
            }
            else {
                for(int i = 0; i < moveDist-1; i++) {
                    solution.pop();
                }
            }
        }
        else {
            solution.push(temp);
            solution.push(internalMap[currX][currY].coords);
        }
        cerr << "Moving to (" << currX << "," << currY << ")" << endl;
        API::moveForward(moveDist);
        totalDist+=moveDist;
    }

    solution.pop(); // Starting cell is popped because you're already on it lol

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

    // Follow the 'solution' stack's path if it is continuous, otherwise search the maze again
    if(solutionCheck(solution)) {
        cerr << "Maze mapping complete!" << endl;
        traverse();
    }
    else {
        cerr << "Retracing maze..." << endl;
        search(true);
    }
}

// Function for moving forward multiple cells at a time across previously visted cells
short Map::lookAhead(short currX, short currY, char dir) {
    short dist = 1;

    while(true) {
        if(internalMap[currX][currY].visited != true) return dist;

        switch (dir) {
        case 'w':
            if(*internalMap[currX][currY].westWall == false && currX > 0) {
                if(internalMap[currX][currY].floodVal > internalMap[currX-1][currY].floodVal) {
                    dist++;
                    currX--;
                    break;
                }
                return dist;
            }
            return dist;
        case 'e':
            if(*internalMap[currX][currY].eastWall == false && currX < 15) {
                if(internalMap[currX][currY].floodVal > internalMap[currX+1][currY].floodVal) {
                    dist++;
                    currX++;
                    break;
                }
                return dist;
            }
            return dist;
        case 's':
            if(*internalMap[currX][currY].southWall == false && currY > 0) {
                if(internalMap[currX][currY].floodVal > internalMap[currX][currY-1].floodVal) {
                    dist++;
                    currY--;
                    break;
                }
                return dist;
            }
            return dist;
        default:
            if(*internalMap[currX][currY].northWall == false && currY < 15) {
                if(internalMap[currX][currY].floodVal > internalMap[currX][currY+1].floodVal) {
                    dist++;
                    currY++;
                    break;
                }
                return dist;
            }
            return dist;
        }
    }
}

// Function to check if the path back to the center is continuous
bool Map::solutionCheck(stack<Coor> trace) {
    while(trace.size() > 1) {
        short tempX = trace.top().x; short tempY = trace.top().y;
        trace.pop();
        if(tempX > trace.top().x) {
            if(internalMap[tempX][tempY].floodVal != internalMap[trace.top().x][trace.top().y].floodVal+(tempX - trace.top().x)) return false;
        }
        else if(tempX < trace.top().x) {
            if(internalMap[tempX][tempY].floodVal != internalMap[trace.top().x][trace.top().y].floodVal+(trace.top().x - tempX)) return false;
        }
        else if(tempY > trace.top().y) {
            if(internalMap[tempX][tempY].floodVal != internalMap[trace.top().x][trace.top().y].floodVal+(tempY - trace.top().y)) return false;
        }
        else {
            if(internalMap[tempX][tempY].floodVal != internalMap[trace.top().x][trace.top().y].floodVal+(trace.top().y - tempY)) return false;
        }
    }
    return true;
}

// Function to traverse maze according to the 'solution' stack
void Map::traverse() {

    cerr << "Starting next run!" << endl;

    short currX = 0;
    short currY = 0;
    char dir = 'n';

    while(internalMap[currX][currY].floodVal != 0) {

        if(API::wasReset()) {
            cerr << "Resetting!" << endl;
            return;
        }

        short stepIndex;
        short moveDist = 0;
        Coor temp = solution.top();
        solution.pop();
        if(currX > temp.x) {
            stepIndex = 0;
            if(!solution.empty()) {
                while (temp.x > solution.top().x) {
                    temp = solution.top();
                    solution.pop();
                    if(solution.empty()) break;
                }
            }
            moveDist = currX - temp.x;
        }
        else if(currX < temp.x) {
            stepIndex = 1;
            if(!solution.empty()) {
                while (temp.x < solution.top().x) {
                    temp = solution.top();
                    solution.pop();
                    if(solution.empty()) break;
                }
            }
            moveDist = temp.x - currX;
        }
        else if(currY > temp.y) {
            stepIndex = 2;
            if(!solution.empty()) {
                while (temp.y > solution.top().y) {
                    temp = solution.top();
                    solution.pop();
                    if(solution.empty()) break;
                }
            }
            moveDist = currY - temp.y;
        }
        else {
            stepIndex = 3;
            if(!solution.empty()) {
                while (temp.y < solution.top().y) {
                    temp = solution.top();
                    solution.pop();
                    if(solution.empty()) break;
                }
            }
            moveDist = temp.y - currY;
        }

        dir = turnMouse(dir,stepIndex);

        switch (stepIndex) {
        case 0:
            currX-=moveDist;
            break;
        case 1:
            currX+=moveDist;
            break;
        case 2:
            currY-=moveDist;
            break;
        default:
            currY+=moveDist;
        }

        cerr << "Moving to (" << currX << "," << currY << ")" << endl;
        API::moveForward(moveDist);
        totalDist++;
    }

    cerr << "Run complete!" << endl;
    cerr << "Speedrun achieved!" << endl;
}

// Function which returns minimum index after 'flooder' is called
short Map::floodStep(Cell init, char dir) {

    flooder(init);

    return findMinIndex(neighborCheck(init),dir);
}

// Function that 'floods' neighboring cells if needed
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

// Function that returns a vector of all accessible neighboring cells' flood values
// (-1 indicates neighbor is inaccessible)
vector<int> Map::neighborCheck(Cell currCell) {
    vector<int> neighbors = {-1,-1,-1,-1};
    if(*currCell.westWall == false) {
        neighbors[0] = internalMap[currCell.coords.x-1][currCell.coords.y].floodVal;
    }
    if(*currCell.eastWall == false) {
        neighbors[1] = internalMap[currCell.coords.x+1][currCell.coords.y].floodVal;
    }
    if(*currCell.southWall == false) {
        neighbors[2] = internalMap[currCell.coords.x][currCell.coords.y-1].floodVal;
    }
    if(*currCell.northWall == false) {
        neighbors[3] = internalMap[currCell.coords.x][currCell.coords.y+1].floodVal;
    }
    return neighbors;
}

// Function to lowest flood value amongst the accessible neighbors
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

// Function to return the index of an accessible neighbor with the lowest flood value
// (Preference is given to neighbors in the 'front' in case of multiple options)
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

// Function to set center walls after reaching one of the center cells
void Map::centerWalls(short currX, short currY, char dir) {
    switch (dir) {
    case 'w':
        if(currX == 8 && currY == 8) {
            API::setWall(8,8,'n');
            *internalMap[8][8].northWall = true;

            API::setWall(7,8,'n');
            *internalMap[7][8].northWall = true;

            API::setWall(7,8,'w');
            *internalMap[7][8].westWall = true;

            API::setWall(7,7,'w');
            *internalMap[7][7].westWall = true;

            API::setWall(7,7,'s');
            *internalMap[7][7].southWall = true;

            API::setWall(8,7,'s');
            *internalMap[8][7].southWall = true;

            API::setWall(8,7,'e');
            *internalMap[8][7].eastWall = true;
        }
        else {
            API::setWall(8,7,'s');
            *internalMap[8][7].southWall = true;

            API::setWall(7,7,'s');
            *internalMap[7][7].southWall = true;

            API::setWall(7,7,'w');
            *internalMap[7][7].westWall = true;

            API::setWall(7,8,'w');
            *internalMap[7][8].westWall = true;

            API::setWall(7,8,'n');
            *internalMap[7][8].northWall = true;

            API::setWall(8,8,'n');
            *internalMap[8][8].northWall = true;

            API::setWall(8,8,'e');
            *internalMap[8][8].eastWall = true;
        }
        break;
    case 'e':
        if(currX == 7 && currY == 7) {
            API::setWall(7,7,'s');
            *internalMap[7][7].southWall = true;

            API::setWall(8,7,'s');
            *internalMap[8][7].southWall = true;

            API::setWall(8,7,'e');
            *internalMap[8][7].eastWall = true;

            API::setWall(8,8,'e');
            *internalMap[8][8].eastWall = true;

            API::setWall(8,8,'n');
            *internalMap[8][8].northWall = true;

            API::setWall(7,8,'n');
            *internalMap[7][8].northWall = true;

            API::setWall(7,8,'w');
            *internalMap[7][8].westWall = true;
        }
        else {
            API::setWall(7,8,'n');
            *internalMap[7][8].northWall = true;

            API::setWall(8,8,'n');
            *internalMap[8][8].northWall = true;

            API::setWall(8,8,'e');
            *internalMap[8][8].eastWall = true;

            API::setWall(8,7,'e');
            *internalMap[8][7].eastWall = true;

            API::setWall(8,7,'s');
            *internalMap[8][7].southWall = true;

            API::setWall(7,7,'s');
            *internalMap[7][7].southWall = true;

            API::setWall(7,7,'w');
            *internalMap[7][7].westWall = true;
        }
        break;
    case 's':
        if(currX == 8 && currY == 8) {
            API::setWall(8,8,'e');
            *internalMap[8][8].eastWall = true;

            API::setWall(8,7,'e');
            *internalMap[8][7].eastWall = true;

            API::setWall(8,7,'s');
            *internalMap[8][7].southWall = true;

            API::setWall(7,7,'s');
            *internalMap[7][7].southWall = true;

            API::setWall(7,7,'w');
            *internalMap[7][7].westWall = true;

            API::setWall(7,8,'w');
            *internalMap[7][8].westWall = true;

            API::setWall(7,8,'n');
            *internalMap[7][8].northWall = true;
        }
        else {
            API::setWall(7,8,'w');
            *internalMap[7][8].westWall = true;

            API::setWall(7,7,'w');
            *internalMap[7][7].westWall = true;

            API::setWall(7,7,'s');
            *internalMap[7][7].southWall = true;

            API::setWall(8,7,'s');
            *internalMap[8][7].southWall = true;

            API::setWall(8,7,'e');
            *internalMap[8][7].eastWall = true;

            API::setWall(8,8,'e');
            *internalMap[8][8].eastWall = true;

            API::setWall(8,8,'n');
            *internalMap[8][8].northWall = true;
        }
        break;
    default:
        if(currX == 7 && currY == 7) {
            API::setWall(7,7,'w');
            *internalMap[7][7].westWall = true;

            API::setWall(7,8,'w');
            *internalMap[7][8].westWall = true;

            API::setWall(7,8,'n');
            *internalMap[7][8].northWall = true;

            API::setWall(8,8,'n');
            *internalMap[8][8].northWall = true;

            API::setWall(8,8,'e');
            *internalMap[8][8].eastWall = true;

            API::setWall(8,7,'e');
            *internalMap[8][7].eastWall = true;

            API::setWall(8,7,'s');
            *internalMap[8][7].southWall = true;
        }
        else {
            API::setWall(8,7,'e');
            *internalMap[8][7].eastWall = true;

            API::setWall(8,8,'e');
            *internalMap[8][8].eastWall = true;

            API::setWall(8,8,'n');
            *internalMap[8][8].northWall = true;

            API::setWall(7,8,'n');
            *internalMap[7][8].northWall = true;

            API::setWall(7,8,'w');
            *internalMap[7][8].westWall = true;

            API::setWall(7,7,'w');
            *internalMap[7][7].westWall = true;

            API::setWall(7,7,'s');
            *internalMap[7][7].southWall = true;
        }
    }
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
