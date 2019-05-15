//
//  MonteCarloData.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef MonteCarloData_hpp
#define MonteCarloData_hpp

#include "GameState.hpp"

typedef std::vector<GameState> GameStateVector;

struct MonteCarloData {
    GameStateVector path;
    std::string prettyString() const {
        std::string result = "";
        result += "\nPath:\n";
        for (int i = 0; i < path.size(); i++) {
            result += path[i].prettyString() + "\n";
        }
        const GameState& finalState = path.back();
        result += "Final Score: " + std::to_string(finalState.getScore()) + "\n";
        return result;
    }
};

#endif /* MonteCarloData_hpp */
