//
//  Game.cpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#include "Game.hpp"

#include <random>

Game::Game(const GameFoundation& gameFoundation) : gameFoundation(gameFoundation) {
    const DominoVector& dominoSet = gameFoundation.dominoSet;
    for (int i = 0; i < dominoSet.size(); i++) {
        shuffledDominoes.push_back(&dominoSet[i]);
    }

    // time-based seed
    const unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(std::begin(shuffledDominoes), std::end(shuffledDominoes), std::default_random_engine(seed));

    const int handSize = gameFoundation.handSize;
    const int numberOfDominoes = gameFoundation.numberOfDominoes;

    // randomly assign hands
    for (int i = 0; i < handSize; i++) {
        maxPlayerHand.push_back(shuffledDominoes[i]);
    }
    for (int i = 0, j = handSize; i < handSize; i++, j++) {
        minPlayerHand.push_back(shuffledDominoes[j]);
    }

    // set initial possible tiles for each player
    for (int i = 0; i < numberOfDominoes; i++) {
        if (i >= handSize) {
            possibleMinPlayerTiles.push_back(shuffledDominoes[i]);
        }
        if (i < handSize || i >= handSize * 2) {
            possibleMaxPlayerTiles.push_back(shuffledDominoes[i]);
        }
    }
}
