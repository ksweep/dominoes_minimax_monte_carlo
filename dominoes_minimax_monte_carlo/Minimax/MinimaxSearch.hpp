//
//  MinimaxSearch.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef MinimaxSearch_hpp
#define MinimaxSearch_hpp

#include "GameState.hpp"
#include "GameFoundation.hpp"
#include "MinimaxData.hpp"

struct MinimaxResult {
    MinimaxResult(GameState *node, int score) {
        this->node = node;
        this->score = score;
    }
    GameState* node;
    int score;
};

class MinimaxSearch {
private:
    static int _minimaxSearch(MinimaxData& data, GameState& gameState, const int depth, const int infinity, const int negativeInfinity);
public:
    static MinimaxData runOnState(GameState& gameState);
};

class AlphaBetaMinimaxSearch {
private:
    static MinimaxResult _alphaBetaMinimaxSearch(MinimaxData& data, GameState& gameState, const int depth, const MinimaxResult alpha, const MinimaxResult beta, const int internalNodeScore);
public:
    static MinimaxData runOnState(GameState& gameState, const bool recordPath);
};

#endif /* MinimaxSearch_hpp */
