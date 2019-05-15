//
//  GameTreeDataGenerator.cpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#include "GameTreeDataGenerator.hpp"

#include <stack>

void GameTreeDataGenerator::_depthFirstSearch(GameTreeData& data, GameState &gameState, const int internalNodeScore) {
    std::stack<GameState*> stack;
    stack.push(&gameState);

    while (!stack.empty()) {
        GameState* currentState = stack.top();
        stack.pop();

        const int score = currentState->getScore();

        if (score == internalNodeScore) {
            currentState->expandAndGenerateChildren();
            data.expandedNodes++;
            const int numberOfChildren = (int)currentState->children.size();
            for (int childIndex = 0; childIndex < numberOfChildren; childIndex++) {
                stack.push(&currentState->children[childIndex]);
            }
            data.childrenSum += numberOfChildren;
        } else {
            data.leafNodes++;
            currentState->minimaxValue = score;
            if (score > data.maxMinimaxValue) {
                data.maxMinimaxValue = score;
            }
            if (score < data.minMinimaxValue) {
                data.minMinimaxValue = score;
            }
            if (currentState->depth < data.minLeafDepth) {
                data.minLeafDepth = currentState->depth;
            }
            if (currentState->depth > data.maxLeafDepth) {
                data.maxLeafDepth = currentState->depth;
            }
        }
    }
}

GameTreeData GameTreeDataGenerator::generateDataFromState(const GameState& state) {
    GameState gameState(state);
    GameTreeData result(gameState.game.gameFoundation);
    _depthFirstSearch(result, gameState, gameState.game.gameFoundation.infinity);
    return result;
}
