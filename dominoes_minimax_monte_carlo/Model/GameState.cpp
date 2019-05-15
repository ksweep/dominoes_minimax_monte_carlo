//
//  GameState.cpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#include "GameState.hpp"

GameState::GameState(const Game& game) : game(game) {
    maxPlayerHand = game.maxPlayerHand;
    minPlayerHand = game.minPlayerHand;
    playerTurnIndex = 0; // start with max player
    layoutEnds = std::make_pair(-1, -1);
    depth = 0;
    minimaxValue = _defaultMinimaxValueForTurnIndex(playerTurnIndex);
    previousPlayerPassed = false;
    jammed = false;
    parent = NULL;
}

GameState GameState::stateRemovingChildren(const GameState &gameState) {
    return GameState(gameState.game,
                     gameState.maxPlayerHand,
                     gameState.minPlayerHand,
                     gameState.playedTiles,
                     gameState.playerTurnIndex,
                     gameState.layoutEnds,
                     gameState.depth,
                     gameState.minimaxValue,
                     gameState.previousPlayerPassed,
                     gameState.jammed,
                     gameState.parent);
}

GameState GameState::stateForGeneratingChildState(const Game& game, GameState* gameState, const bool statePasses, const int layoutValue, const int tileIndex, const DominoPointerVector& playerHand) {
    return GameState(game,
                     gameState,
                     statePasses,
                     layoutValue,
                     tileIndex,
                     playerHand);
}

GameState GameState::stateForMonteCarloSampling(const GameState &gameState) {
    return GameState(gameState.game,
                     gameState.isMaxPlayer() ? gameState.maxPlayerHand : gameState.minPlayerHand,
                     gameState.playedTiles,
                     gameState.playerTurnIndex,
                     gameState.layoutEnds,
                     gameState.depth,
                     gameState.previousPlayerPassed,
                     gameState.jammed,
                     gameState.parent);
}

// for not copying children
GameState::GameState(const Game& game,
                     const DominoPointerVector& maxPlayerHand,
                     const DominoPointerVector& minPlayerHand,
                     const DominoPointerVector& playedTiles,
                     const int playerTurnIndex,
                     const IntPair layoutEnds,
                     const int depth,
                     const int minimaxValue,
                     const bool previousPlayerPassed,
                     const bool jammed,
                     GameState* parent) : game(game) {
    this->maxPlayerHand = maxPlayerHand;
    this->minPlayerHand = minPlayerHand;
    this->playedTiles = playedTiles;
    this->playerTurnIndex = playerTurnIndex;
    this->layoutEnds = layoutEnds;
    this->depth = depth;
    this->minimaxValue = minimaxValue;
    this->previousPlayerPassed = previousPlayerPassed;
    this->jammed = jammed;
    this->parent = parent;
}

GameState::GameState(const Game& game,
                     const DominoPointerVector& hand,
                     const DominoPointerVector& playedTiles,
                     const int playerTurnIndex,
                     const IntPair layoutEnds,
                     const int depth,
                     const bool previousPlayerPassed,
                     const bool jammed,
                     GameState* parent) : game(game) {
    this->playedTiles = playedTiles;
    this->playerTurnIndex = playerTurnIndex;
    this->layoutEnds = layoutEnds;
    this->depth = depth;
    this->previousPlayerPassed = previousPlayerPassed;
    this->jammed = jammed;
    this->parent = parent;
    this->minimaxValue = _defaultMinimaxValueForTurnIndex(playerTurnIndex);
    if (isMaxPlayer()) {
        this->maxPlayerHand = hand;
    } else {
        this->minPlayerHand = hand;
    }
}

// for generating a child state
GameState::GameState(const Game& game, GameState* gameState, const bool statePasses, const int layoutValue, const int tileIndex, const DominoPointerVector& playerHand) : game(game) {
    maxPlayerHand = gameState->maxPlayerHand;
    minPlayerHand = gameState->minPlayerHand;
    playedTiles = gameState->playedTiles;
    playerTurnIndex = gameState->playerTurnIndex;
    layoutEnds = gameState->layoutEnds;
    minimaxValue = _defaultMinimaxValueForTurnIndex(playerTurnIndex);
    previousPlayerPassed = false;
    jammed = false;
    parent = gameState;

    if (statePasses) {
        previousPlayerPassed = true;
        if (gameState->previousPlayerPassed) {
            jammed = true;
        }
    } else {
        Domino const* domino = playerHand[tileIndex];
        playedTiles.push_back(domino);
        _updateLayoutEnds(domino, layoutValue);
        DominoPointerVector& handToUpdate = _currentPlayerHand();
        handToUpdate.erase(handToUpdate.begin() + tileIndex);
    }

    depth = gameState->depth + 1;
    playerTurnIndex = 1 - playerTurnIndex;
    minimaxValue = _defaultMinimaxValueForTurnIndex(playerTurnIndex);
}

int GameState::_getPlayerPointTotal(const bool isMaxPlayer) const {
    const DominoPointerVector& hand = isMaxPlayer ? maxPlayerHand : minPlayerHand;
    int total = 0;
    for (int i = 0; i < hand.size(); i++) {
        total += hand[i]->getTileTotal();
    }
    return total;
}

Domino const* GameState::_getPlayerSmallestValueDomino(const bool isMaxPlayer) const {
    const DominoPointerVector& hand = isMaxPlayer ? maxPlayerHand : minPlayerHand;
    int min = game.gameFoundation.infinity;
    Domino const* result = NULL;
    for (int i = 0; i < hand.size(); i++) {
        if (hand[i]->getTileTotal() < min) {
            min = hand[i]->getTileTotal();
            result = hand[i];
        }
    }
    return result;
}

int GameState::_getPlayerSmallestValue(const bool isMaxPlayer) const {
    Domino const* smallestDomino = _getPlayerSmallestValueDomino(isMaxPlayer);
    return smallestDomino->getTileTotal(); // should not be NULL
}

int GameState::_getPlayerSmallestSuitOfSmallestValue(const bool isMaxPlayer) const {
    Domino const* smallestDomino = _getPlayerSmallestValueDomino(isMaxPlayer);
    return smallestDomino->smallerSuit(); // should not be NULL
}

int GameState::getScore() const {
    const int playerHandSize = (int)maxPlayerHand.size();
    const int minPlayerHandSize = (int)minPlayerHand.size();

    if (playerHandSize == 0 && minPlayerHandSize > 0) {
        return _getPlayerPointTotal(false);
    }

    if (minPlayerHandSize == 0 && playerHandSize > 0) {
        return -1 * _getPlayerPointTotal(true);
    }

    if (jammed) {
        const int maxPlayerPointTotal = _getPlayerPointTotal(true);
        const int minPlayerPointTotal = _getPlayerPointTotal(false);

        if (maxPlayerPointTotal < minPlayerPointTotal) {
            return minPlayerPointTotal;
        }
        if (maxPlayerPointTotal > minPlayerPointTotal) {
            return -1 * maxPlayerPointTotal;
        }

        // tiebreaker -- smallest tile wins
        const int maxPlayerSmallestValue = _getPlayerSmallestValue(true);
        const int minPlayerSmallestValue = _getPlayerSmallestValue(false);

        if (maxPlayerSmallestValue < minPlayerSmallestValue) {
            return minPlayerPointTotal;
        }
        if (maxPlayerSmallestValue > minPlayerSmallestValue) {
            return -1 * maxPlayerPointTotal;
        }

        // super tiebreaker -- find smallest tile with smallest value
        const int maxPlayerSmallestSuitOfSmallestValue = _getPlayerSmallestSuitOfSmallestValue(true);
        const int minPlayerSmallestSuitOfSmallestValue = _getPlayerSmallestSuitOfSmallestValue(false);

        if (maxPlayerSmallestSuitOfSmallestValue < minPlayerSmallestSuitOfSmallestValue) {
            return minPlayerSmallestValue;
        }

        assert(maxPlayerSmallestSuitOfSmallestValue > minPlayerSmallestSuitOfSmallestValue);

        return -1 * maxPlayerPointTotal;
    }

    // state isn't a leaf node
    return game.gameFoundation.infinity;
}

void GameState::expandAndGenerateChildren() {
    bool pass = true;

    DominoPointerVector& currentPlayerHand = _currentPlayerHand();

    if (playedTiles.size() == 0) {
        for (int i = 0; i < currentPlayerHand.size(); i++) {
            pass = false;
            _generateState(i, currentPlayerHand, -1, pass);
        }
    } else {
        for (int i = 0; i < currentPlayerHand.size(); i++) {
            if (layoutEnds.first == layoutEnds.second) {
                if (currentPlayerHand[i]->hasSuit(layoutEnds.first)) {
                    pass = false;
                    _generateState(i, currentPlayerHand, layoutEnds.first, pass);
                }
            } else {
                if (currentPlayerHand[i]->hasSuit(layoutEnds.first)) {
                    pass = false;
                    _generateState(i, currentPlayerHand, layoutEnds.first, pass);
                }
                if (currentPlayerHand[i]->hasSuit(layoutEnds.second)) {
                    pass = false;
                    _generateState(i, currentPlayerHand, layoutEnds.second, pass);
                }
            }
        }
    }

    if (pass) {
        _generateState(0, currentPlayerHand, layoutEnds.second, true);
    }
}

void GameState::_updateLayoutEnds(Domino const* domino, const int layoutValue) {
    if (playedTiles.size() == 1) {
        layoutEnds.first = domino->ends.first;
        layoutEnds.second = domino->ends.second;
    } else {
        if (layoutValue == layoutEnds.first) {
            if (domino->ends.first == layoutEnds.first){
                layoutEnds.first = domino->ends.second;
            } else if (domino->ends.second == layoutEnds.first) {
                layoutEnds.first = domino->ends.first;
            }
        } else { // layoutValue == layoutEnds.second
            if (domino->ends.first == layoutEnds.second) {
                layoutEnds.second = domino->ends.second;
            } else if (domino->ends.second == layoutEnds.second) {
                layoutEnds.second = domino->ends.first;
            }
        }
    }
}

void GameState::_generateState(const int tileIndex, const DominoPointerVector& playerHand, const int layoutValue, const bool pass) {
    GameState childState = GameState::stateForGeneratingChildState(game, this, pass, layoutValue, tileIndex, playerHand);
    children.push_back(childState);
}

std::string GameState::prettyString() const {
    std::string result = "";

    result += "Depth: " + std::to_string(depth) + "\n";

    std::string playerString = isMaxPlayer() ? "Max Player" : "Min Player";
    result += "Player Turn: " + playerString + "\n";

    result += "Max Hand:";
    for (int i = 0; i < maxPlayerHand.size(); i++) {
        result += " " + maxPlayerHand[i]->prettyString();
    }
    result += "\n";

    result += "Min Hand:";
    for (int i = 0; i < minPlayerHand.size(); i++) {
        result += " " + minPlayerHand[i]->prettyString();
    }
    result += "\n";

    result += "Played Tiles:";
    for (int i = 0; i < playedTiles.size(); i++) {
        result += " " + playedTiles[i]->prettyString();
    }
    result += "\n";

    result += "Layout Ends: " + std::to_string(layoutEnds.first) + ", " + std::to_string(layoutEnds.second) + "\n";

    result += "Previous Player Passed: " + std::to_string(previousPlayerPassed) + "\n";

    result += "Jammed: " + std::to_string(jammed) + "\n";

    result += "Minimax Score: " + std::to_string(minimaxValue) + "\n";

    return result;
}
