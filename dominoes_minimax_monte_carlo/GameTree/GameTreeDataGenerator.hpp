//
//  GameTreeDataGenerator.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef GameTreeDataGenerator_hpp
#define GameTreeDataGenerator_hpp

#include <stdio.h>

#include "GameState.hpp"
#include "GameTreeData.hpp"

class GameTreeDataGenerator {
private:
    static void _depthFirstSearch(GameTreeData& data, GameState &gameState, const int internalNodeScore);
public:
    static GameTreeData generateDataFromState(const GameState& state);
};

#endif /* GameTreeDataGenerator_hpp */
