//
//  MinimaxSearch.cpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#include "MinimaxSearch.hpp"

int MinimaxSearch::_minimaxSearch(MinimaxData& data, GameState& gameState, const int depth, const int infinity, const int negativeInfinity) {
    const int score = gameState.getScore();

    data.nodesVisited += 1;

    if (depth == 0 || score != infinity) {
        gameState.minimaxValue = score;
        return score;
    }

    gameState.expandAndGenerateChildren();

    const int numberOfChildren = (int)gameState.children.size();

    if (gameState.isMaxPlayer()) {
        int value = negativeInfinity;
        for (int childIndex = 0; childIndex < numberOfChildren; childIndex++) {
            value = std::max(value, _minimaxSearch(data, gameState.children[childIndex], depth - 1, infinity, negativeInfinity));
            gameState.minimaxValue = value;
        }
        return value;
    }

    int value = infinity;
    for (int childIndex = 0; childIndex < numberOfChildren; childIndex++) {
        value = std::min(value, _minimaxSearch(data, gameState.children[childIndex], depth - 1, infinity, negativeInfinity));
        gameState.minimaxValue = value;
    }
    return value;
}

MinimaxData MinimaxSearch::runOnState(GameState& gameState) {
    const GameFoundation& gameFoundation = gameState.game.gameFoundation;
    MinimaxData result;
    _minimaxSearch(result, gameState, gameFoundation.maxDepth, gameFoundation.infinity, gameFoundation.negativeInfinity);
    result.minimaxValue = gameState.minimaxValue;
    return result;
}

MinimaxResult AlphaBetaMinimaxSearch::_alphaBetaMinimaxSearch(MinimaxData& data, GameState& gameState, const int depth, const MinimaxResult alpha, const MinimaxResult beta, const int internalNodeScore) {
    const int score = gameState.getScore();

    data.nodesVisited += 1;

    if (depth == 0 || score != internalNodeScore) {
        gameState.minimaxValue = score;
        return MinimaxResult(&gameState, score);
    }

    gameState.expandAndGenerateChildren();

    const int numberOfChildren = (int)gameState.children.size();

    if (gameState.isMaxPlayer()) {
        MinimaxResult alphaResult = alpha;
        for (int childIndex = 0; childIndex < numberOfChildren; childIndex++) {
            const MinimaxResult recursiveAlpha = _alphaBetaMinimaxSearch(data, gameState.children[childIndex], depth - 1, alphaResult, beta, internalNodeScore);
            if (recursiveAlpha.score > alphaResult.score) {
                alphaResult = recursiveAlpha;
            }
            gameState.minimaxValue = alphaResult.score;
            if (beta.score <= alphaResult.score) {
                break;
            }
        }
        return alphaResult;
    }

    MinimaxResult betaResult = beta;
    for (int childIndex = 0; childIndex < numberOfChildren; childIndex++) {
        const MinimaxResult recursiveBeta = _alphaBetaMinimaxSearch(data, gameState.children[childIndex], depth - 1, alpha, betaResult, internalNodeScore);
        if (recursiveBeta.score < betaResult.score) {
            betaResult = recursiveBeta;
        }
        gameState.minimaxValue = betaResult.score;
        if (betaResult.score <= alpha.score) {
            break;
        }
    }
    return betaResult;
}

MinimaxData AlphaBetaMinimaxSearch::runOnState(GameState& gameState, const bool recordPath) {
    const GameFoundation& gameFoundation = gameState.game.gameFoundation;

    MinimaxData result;

    MinimaxResult alphaBetaResult = _alphaBetaMinimaxSearch(result, gameState, gameFoundation.maxDepth, MinimaxResult(NULL, gameFoundation.negativeInfinity), MinimaxResult(NULL, gameFoundation.infinity), gameFoundation.infinity);

    if (recordPath) {
        GameState *node = alphaBetaResult.node;
        while (node != NULL) {
            GameState nodeRemovingChildren = GameState::stateRemovingChildren(*node);
            result.path.push_back(nodeRemovingChildren);
            node = node->parent;
        }
    }

    assert(alphaBetaResult.score == alphaBetaResult.node->minimaxValue);
    assert(!recordPath || alphaBetaResult.node->depth + 1 == (int)result.path.size());

    result.minimaxValue = alphaBetaResult.score;

    return result;
}
