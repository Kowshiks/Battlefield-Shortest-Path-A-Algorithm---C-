#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <unordered_set>
#include <fstream> 
#include "nlohmann/json.hpp"
#include <random>
#include <string>

using json = nlohmann::json;

// Size of Row and Col of the Battleship Grid
int rows;
int cols;

// THe values of the Riskylab JSON Format file
double elevatedTerrainValue = 4;
double grounndTerrainValue = -1;
double startingValue = 6.4;
double targetValue = 0.6;

// Battleship Grid
std::vector<std::vector<double>> battlefieldGrid;

std::unordered_set<std::string> visited;

// Position is a structure that defines the properties of each unit movement
struct Position {
    
    // x and y are the coorsinates of the unit
    int x;
    int y;
    
    int prev_cost; // the cost until the current move
    int heuristic; // heuristic is the manhattan distance to the goal state
    int tot_cost; // total cost = prev_cost + heuristic
    std::vector<std::pair<int, int>> eachVisit; // contains all the visited path

    // Constructor
    Position(int x, int y, int prev, int heu, int tot, const std::vector<std::pair<int, int>>& vis) :
        x(x), y(y), prev_cost(prev), heuristic(heu), tot_cost(tot), eachVisit(vis) {}

    // This is used to denote the priority on the tot_cost value
    bool operator>(const Position& other) const {
        return tot_cost > other.tot_cost;
    }
    
};

// function to calculate the manhattan distance
double distCalc(int x1, int y1, int x2, int y2) {
    
    double val = std::abs(x1 - x2) + std::abs(y1 - y2);
    
    return val;
    
}

// function to check if a fiven x,y coordinates are valid
bool valid(int x, int y) {
    
    // checks if x is in range
    if (x < 0 || x >= rows){
        
        return false;
    }
    
    // checks if y is in range
    if (y < 0 || y >= cols){
        
        return false;
    }
    
    // checks if the grid value of x,y is the unreacheable elevatedTerrainValue (4)
    if (battlefieldGrid[x][y] == elevatedTerrainValue){
        
        return false;
    }
    
    // checks if the x,y coordintes are already in the visited set
    if (visited.find(std::to_string(x) + "," + std::to_string(y)) != visited.end()){
                
        return false;
    }
    
    return true;
}

// function to calculate the shortest path from a starting unit to a target unit
std::vector<std::pair<int, int>> shortestPath(const std::vector<std::vector<double>>& grid,
                                                       const std::pair<int, int>& start,
                                                       const std::pair<int, int>& target) {
        
        // moves contains all the vertical and horizontal discrete moves
        const std::vector<std::vector<int>> moves = {
        {-1, 0},  
        {1, 0},   
        {0, -1},  
        {0, 1}    
        };
        
        // a priority queue called fringe is initialized
        std::priority_queue<Position, std::vector<Position>, std::greater<Position>> fringe;
        
        std::vector<std::pair<int, int>> curVisit;
        
        curVisit.emplace_back(start.first, start.second);
        
        Position cur = Position(start.first, start.second, 0, 0, 0, curVisit);
        
        fringe.push(cur);
        
        // checks until the fringe is empty
        while (!fringe.empty()){
            
            Position node = fringe.top();
            
            fringe.pop();
            
            // checks if the goal state is reached
            if (node.x == target.first && node.y == target.second){
                
                // return the path to the goal state
                return node.eachVisit;
                
            }
            
            // the current position is updated in the visited set
            visited.insert(std::to_string(node.x) + "," + std::to_string(node.y));
            
            // loop through the next set of movevs
            for(std::vector<int> move: moves){
                
                int next_x = node.x + move[0];
                int next_y = node.y + move[1];
                
                // checks if the moves are valid
                if (valid(next_x, next_y)){
                                        
                    std::vector<std::pair<int, int>> interVisit = node.eachVisit;
                    
                    interVisit.emplace_back(next_x, next_y);
                    
                    // the manhattan distance to the goal stae is calculated
                    double cur_heu = distCalc(next_x, next_y, target.first, target.second);
                    
                    // the total cost would be the prev cost + heuristics
                    int tot_cur = node.prev_cost + cur_heu;
                    
                    Position curNode = Position(next_x, next_y, node.prev_cost+1, cur_heu, tot_cur, interVisit);
                    
                    // the current position properties are appended to the fringe
                    fringe.push(curNode);
                    
                }
                
            }
            
        }
        
        // if there is no path then it returns an empty vecror
        return {};
                                                        
    }
    

// function to convert a flattened 1D array into a 2D array of the grid
bool convert1Dto2D(json data){
    
    // checks if the size of grid equates to the row value and column value privided
    if (rows * cols != data.size()) {
    
        std::cerr << "Error: Invalid number of rows and columns." << std::endl;
        return false;
    }
    
    battlefieldGrid.reserve(rows);
    
    for (int i = 0; i < rows; ++i) {
        
        // Intitializes a range of elements into row
        std::vector<double> row(data.begin() + i * cols, data.begin() + (i + 1) * cols);
        
        // push each row into the battlefieldGrid
        battlefieldGrid.push_back(row);
    }
    
    return true;
    
}

int main(int argc, char **argv)
{   
    
    std::cout<<std::endl;
    
    // Get the number of rows for the Battleship grid
    std::cout << "Enter the number of rows of the battlefield: ";
    std::cin >> rows;
    
    std::cout<<std::endl;
    
    // Get the number of columns for the Battleship grid
    std::cout << "Enter the number of columns of the battlefield: ";
    std::cin >> cols;
    
    std::cout<<std::endl;
    
    // 2 ways of inseting the flattended grid data
    std::cout << "Enter 1 if you want to upload the Riskylab JSON Format file"<<std::endl;
    std::cout << "                          OR                               "<<std::endl;
    std::cout << "Enter 2 if you want to manually enter the flattened battlefield"<<std::endl;
    
    std::cout<<std::endl;
    
    std::pair<int, int> startIndex;
    std::pair<int, int> targetIndex;
    
    int val;
    
    std::cin>> val;
    
    std::cin.ignore();
    
    // this is for using the json file
    if (val == 1) {
        
        std::cout<<std::endl;
        
        std::string filePath;
        std::cout << "Enter the path of the JSON file: ";
        std::cin >> filePath;
        
        std::cout<<std::endl;

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error opening file." << std::endl;
            return 1;
        }

        json mapData;
        file >> mapData;

        file.close();
        
        json dataField;
        
        // extract the data out of the json file
        if (mapData.contains("layers") && mapData["layers"].size() > 0 && mapData["layers"][0].contains("data")) {
            
            dataField = mapData["layers"][0]["data"];
            
        } else {
            std::cerr << "Error: Invalid JSON structure. Unable to find the 'data' field." << std::endl;
            return 1;
        }
        
        // function to convert the 1D array to 2D
        bool convert = convert1Dto2D(dataField);
        
        if (!convert){
            
            return 1;
        }
        
        // get the coordinates of the starting point (6.4) and the target point (0.6)
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                
                if (battlefieldGrid[i][j] == startingValue) {
                    startIndex = {i, j};
                    
                } else if (battlefieldGrid[i][j] == targetValue) {
                    targetIndex = {i, j};
                }
            }
        }
        
    }
    
    // this is for manually inputting the data
    else if (val == 2){
        
        std::cout<<std::endl;
        
        std::cout<<"Enter the flattened 1D array (should include only -1 and 4 !! for Example: [-1,-1,4,4,-1,-1]): ";
        
        std::string arr;
        
        // Manually enter the array
        std::getline(std::cin, arr);
        
        std::cout<<std::endl;

        try {
            json dataField = json::parse(arr);
            
            if (dataField.is_array()) {

                bool convert = convert1Dto2D(dataField);
            
                if (!convert){
                    
                    return 1;
                }
            
            
            } else {
                std::cerr << "Error: Input is not a JSON array." << std::endl;
                return 1;
            }
            
            // 2 ways of deciding the starting and target points
            std::cout<<"Enter 1 if you want to MANUALLY INPUT the Starting & Target points"<<std::endl;
            std::cout<<"                           OR                            "<<std::endl;
            std::cout<<"Enter 2 if you want to RANDOMLY GENERATE the Starting & Target points"<<std::endl;
            
            std::cout<<std::endl;
            
            int inputVal;
    
            std::cin>> inputVal;
            
            std::cout<<std::endl;
            
            int index_x1,index_y1,index_x2,index_y2;
            
            // manually provide the starting and ending coordinates
            if (inputVal == 1){
                
                std::cout<<"Make sure the starting & target points are not on the elevated terrain (4)"<<std::endl;
                std::cout<<std::endl;
                std::cout<<"Enter the x coordinate of the starting point: ";
                std::cin>> index_x1;
                std::cout<<std::endl;
                std::cout<<"Enter the y coordinate of the starting point: ";
                std::cin>> index_y1;
                std::cout<<std::endl;
                std::cout<<"Enter the x coordinate of the target point: ";
                std::cin>> index_x2;
                std::cout<<std::endl;
                std::cout<<"Enter the y coordinate of the target point: ";
                std::cin>> index_y2;
                std::cout<<std::endl;
                
                // Throw error message of the coordinates are not in the range
                if ((index_x1 < 0 || index_x1 >= rows) || (index_x2 < 0 || index_x2 >= rows)){
                    
                    std::cerr << "Error: The coordinates are not in the range" <<std::endl;
                    return 1;
                }
                
                else if ((index_y1 < 0 || index_y1 >= cols) || (index_y2 < 0 || index_y2 >= cols)){
                    
                    std::cerr << "Error: The coordinates are not in the range" <<std::endl;
                    return 1;
                }
                
            }
            
            // randomly generate the starting and target coorinates
            else if (inputVal == 2){
                
                std::random_device rd;
                std::mt19937 gen(rd());
                
                int rowVal = static_cast<int>(battlefieldGrid.size());
                int colVal = static_cast<int>(battlefieldGrid[0].size());
                
                std::uniform_int_distribution<int> distribution_row(0, rowVal - 1);
                std::uniform_int_distribution<int> distribution_col(0, colVal - 1);
                
                int cnt = 0;
                
                // check for valid starting point unit until a certain limit
                do {
                    index_x1 = distribution_row(gen);
                    index_y1 = distribution_col(gen);
                    
                    if (cnt > rows*cols*2){
                        std::cerr << "Error: Couldnt find a valid ground terrain" <<std::endl;
                        return 1;
                    }
                    
                    cnt+=1;
                    
                } while (battlefieldGrid[index_x1][index_y1] != -1);
                
                cnt = 0;
                
                // check for valid target point unit until a certain limit
                do {
                    index_x2 = distribution_row(gen);
                    index_y2 = distribution_col(gen);
                    
                    if (cnt > rows*cols*2){
                        std::cerr << "Error: Couldnt find a valid ground terrain" << std::endl;
                        return 1;
                    }
                    
                    cnt+=1;
                    
                } while (battlefieldGrid[index_x2][index_y2] != -1 || (index_x1 == index_x2 && index_y1 == index_y2));
                
            }
            
            else {
                
                std::cerr << "Error: Invalid input" << std::endl;
                return 1;
                
            }
            
            // Intitalize the start and the target index
            startIndex = {index_x1, index_y1};
            targetIndex = {index_x2, index_y2};
            
            
            
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            return 1;
        }
        
    }
    
    else {
        
        std::cerr << "Error: Invalid input" << std::endl;
        return 1;
    }
    
    // call the shortest path function with the grid, startIndex and the targetIndex
    std::vector<std::pair<int, int>> tmp = shortestPath(battlefieldGrid, startIndex, targetIndex); 
    
    // if empty throw notification of no path exists.
    if (tmp.empty()){
        
        std::cout << "There are no valid paths for the battle unit to travel from the starting position to the target";
        return 1;
    }
    
    std::string arrow = "";
        
    std::cout << "The shortest path to reach the target is:" << std::endl;
    
    std::cout<<std::endl;
    
    // print the shortest path
    for (const auto& pair : tmp) {
        std::cout<<arrow;
        std::cout << "(" << pair.first << ", " << pair.second << ")";
        arrow = " -> ";
    }
    
    std::cout<<std::endl;
    
	return 0;
}
