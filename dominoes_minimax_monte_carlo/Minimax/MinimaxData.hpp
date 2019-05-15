//
//  MinimaxData.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef MinimaxData_hpp
#define MinimaxData_hpp

#include <stdio.h>

typedef std::vector<GameState> GameStateVector;

struct MinimaxData {
    MinimaxData(int nodesVisited = 0, int minimaxValue = 0) : nodesVisited(nodesVisited), minimaxValue(minimaxValue) {}

    int nodesVisited;
    int minimaxValue;
    GameStateVector path;

    std::string prettyString(const bool printPath) const {
        std::string result = "";
        result += "Nodes Visited: " + std::to_string(nodesVisited) + "\n";
        result += "Minimax Value: " + std::to_string(minimaxValue) + "\n";
        if (printPath) {
            result += "\nPath:\n";
            for (int i = (int)path.size() - 1; i >= 0; i--) {
                result += path[i].prettyString() + "\n";
            }
        }
        return result;
    }
};

#endif /* MinimaxData_hpp */
