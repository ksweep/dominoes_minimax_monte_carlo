//
//  Game.cpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#include <random>

#include "Game.hpp"
#include "CombinationListCreator.hpp"

Game Game::randomGame(const GameFoundation &gameFoundation) {
    return Game(gameFoundation);
}

Game Game::gameWithStartingMaxPayerHandNumber(const GameFoundation &gameFoundation, const int maxPlayerHandNumber) {
    return Game(gameFoundation, maxPlayerHandNumber);
}

Game::Game(const GameFoundation& gameFoundation) : gameFoundation(gameFoundation) {
    const DominoVector& dominoSet = gameFoundation.dominoSet;
    const int numberOfDominoes = gameFoundation.numberOfDominoes;
    const int handSize = gameFoundation.handSize;

    // make a shuffled set for assigning
    DominoPointerVector dominoes;
    for (int i = 0; i < dominoSet.size(); i++) {
        dominoes.push_back(&dominoSet[i]);
    }
    const unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count(); // time-based seed
    std::shuffle(std::begin(dominoes), std::end(dominoes), std::default_random_engine(seed));

    _assignHands(handSize, numberOfDominoes, dominoes);
}

Game::Game(const GameFoundation& gameFoundation, const int maxPlayerHandNumber) : gameFoundation(gameFoundation) {
    const DominoVector& dominoSet = gameFoundation.dominoSet;
    const int numberOfDominoes = gameFoundation.numberOfDominoes;
    const int handSize = gameFoundation.handSize;

    const IntVectorVector indexCombos = CombinationListCreator().listOfCombinationIndices(gameFoundation.numberOfDominoes, gameFoundation.handSize);
    const int handIndex = std::max(0, std::min(maxPlayerHandNumber, (int)indexCombos.size()));
    const IntVector& maxPlayerIndexList = indexCombos[handIndex];

    IntVector dominoIndexList = maxPlayerIndexList;
    for (int i = 0; i < numberOfDominoes; i++) {
        if (std::find(dominoIndexList.begin(), dominoIndexList.end(), i) == dominoIndexList.end()) {
            dominoIndexList.push_back(i);
        }
    }
    // shuffle indices after the max player hand indices
    const unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count(); // time-based seed
    std::shuffle(std::begin(dominoIndexList) + handSize, std::end(dominoIndexList), std::default_random_engine(seed));
    assert(dominoIndexList.size() == dominoSet.size());

    DominoPointerVector dominoes;
    for (int i = 0; i < dominoIndexList.size(); i++) {
        dominoes.push_back(&dominoSet[dominoIndexList[i]]);
    }

    _assignHands(handSize, numberOfDominoes, dominoes);
}

void Game::_assignHands(const int handSize, const int numberOfDominoes, DominoPointerVector& dominoes) {
    // assign hands
    for (int i = 0; i < handSize; i++) {
        maxPlayerHand.push_back(dominoes[i]);
    }
    for (int i = 0, j = handSize; i < handSize; i++, j++) {
        minPlayerHand.push_back(dominoes[j]);
    }

    // set initial possible tiles for each player
    for (int i = 0; i < numberOfDominoes; i++) {
        if (i >= handSize) {
            possibleMinPlayerTiles.push_back(dominoes[i]);
        }
        if (i < handSize || i >= handSize * 2) {
            possibleMaxPlayerTiles.push_back(dominoes[i]);
        }
    }
}
