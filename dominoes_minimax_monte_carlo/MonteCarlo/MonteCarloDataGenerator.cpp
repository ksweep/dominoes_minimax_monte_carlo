//
//  MonteCarloDataGenerator.cpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#include <thread>
#include <random>

#include "MonteCarloDataGenerator.hpp"

#include "CombinationListCreator.hpp"
#include "MinimaxSearch.hpp"

void MonteCarloDataGenerator::_updateChildCounts(const bool isMaxPlayer, const GameFoundation& gameFoundation, GameState& workState, IntVector& childCounts) {
    int currentMinimaxValue = isMaxPlayer ? gameFoundation.negativeInfinity : gameFoundation.infinity;
    int bestChildIndex = -1;
    for (int i = 0; i < workState.children.size(); i++) {
        const int childMinimaxValue = workState.children[i].minimaxValue;
        if ((isMaxPlayer && childMinimaxValue > currentMinimaxValue) ||
            (!isMaxPlayer && childMinimaxValue < currentMinimaxValue)) {
            currentMinimaxValue = childMinimaxValue;
            bestChildIndex = i;
        }
    }

    childCounts[bestChildIndex] = childCounts[bestChildIndex] + 1;
}

int MonteCarloDataGenerator::_getBestChildIndex(IntVector& childCounts) {
    int maxOccurences = 0;
    int indexToUse = -1;
    for (int i = 0; i < childCounts.size(); i++) {
        if (childCounts[i] > maxOccurences) {
            maxOccurences = childCounts[i];
            indexToUse = i;
        }
    }
    return indexToUse;
}

void MonteCarloDataGenerator::_processSamples(GameState& workState, IntVector& childCounts, const int startIndex, const int endIndex, const IntVectorVector& indexCombos, const bool isMaxPlayer, const int opponentHandSize, const DominoPointerVector& possibleTiles, const GameFoundation& gameFoundation) {
    for (int currentSampleNumber = startIndex; currentSampleNumber < endIndex; currentSampleNumber++) {
        const IntVector& indexCombo = indexCombos[currentSampleNumber];
        workState.children.clear();
        // set opponent hand to the sample hand
        assert(indexCombo.size() == opponentHandSize);
        DominoPointerVector& opponentHand = isMaxPlayer ? workState.minPlayerHand : workState.maxPlayerHand;
        opponentHand.clear();
        for (int i = 0; i < opponentHandSize; i++) {
            opponentHand.push_back(possibleTiles[indexCombo[i]]);
        }
        // alpha-beta this state. this will generate children and assign scores.
        MinimaxData alphaBetaData = AlphaBetaMinimaxSearch::runOnState(workState, false);
        assert(workState.children.size() > 0);
        // find child with best score and increment its count
        _updateChildCounts(isMaxPlayer, gameFoundation, workState, childCounts);
        // if there is only one move (i.e. child), do not need to sample anymore since that's the move we must make
        if (workState.children.size() == 1) {
            break;
        }
    }
}

void MonteCarloDataGenerator::_addNewStateToPathAndUpdatePossibleTiles(const GameState& fromState, const GameState& toState, const GameState& mostRecentState, Game& game, MonteCarloData& data) {
    GameState stateToAdd(mostRecentState);

    const bool isMaxPlayer = fromState.isMaxPlayer();

    bool passed = false;

    // determine if player passed and remove layout ends from possible tiles if so
    if (fromState.children.size() == 1) {
        const DominoPointerVector& fromHand = isMaxPlayer ? fromState.maxPlayerHand : fromState.minPlayerHand;
        const DominoPointerVector& toHand = isMaxPlayer ? toState.maxPlayerHand : toState.minPlayerHand;
        if (fromHand.size() == toHand.size()) {
            passed = true;
            DominoPointerVector& possibleTiles = isMaxPlayer ? game.possibleMaxPlayerTiles : game.possibleMinPlayerTiles;
            for (int i = 0; i < possibleTiles.size(); i++) {
                if (possibleTiles[i]->hasSuit(fromState.layoutEnds.first) || possibleTiles[i]->hasSuit(fromState.layoutEnds.second)) {
                    possibleTiles.erase(possibleTiles.begin() + i);
                    i--;
                }
            }
        }
    }

    // modify the copied state (stateToAdd) to represent the recommended move
    stateToAdd.previousPlayerPassed = passed;
    if (passed) {
        if (fromState.previousPlayerPassed) {
            stateToAdd.jammed = true;
        }
    } else {
        Domino const* recommendedDomino = toState.playedTiles.back();
        assert(recommendedDomino != NULL);

        // set the played tiles (implicitly adding recommended tile)
        stateToAdd.playedTiles = toState.playedTiles;

        // update layout ends
        stateToAdd.layoutEnds = toState.layoutEnds;

        // removed played tile from hand
        DominoPointerVector& handToUpdate = isMaxPlayer ? stateToAdd.maxPlayerHand : stateToAdd.minPlayerHand;
        const int oldHandToUpdateSize = (int)handToUpdate.size();
        for (int i = 0; i < handToUpdate.size(); i++) {
            if (handToUpdate[i] == recommendedDomino) {
                handToUpdate.erase(handToUpdate.begin() + i);
                break;
            }
        }
        assert((int)handToUpdate.size() == oldHandToUpdateSize - 1);

        // update possible tiles
        DominoPointerVector& possibleTiles = isMaxPlayer ? game.possibleMaxPlayerTiles : game.possibleMinPlayerTiles;
        const int oldPossibleTilesSize = (int)possibleTiles.size();
        for (int i = 0; i < possibleTiles.size(); i++) {
            if (possibleTiles[i] == recommendedDomino) {
                possibleTiles.erase(possibleTiles.begin() + i);
                break;
            }
        }
        assert((int)possibleTiles.size() == oldPossibleTilesSize - 1);
    }

    // update turn and minimax score
    const int newTurnIndex = 1 - mostRecentState.playerTurnIndex;
    stateToAdd.playerTurnIndex = newTurnIndex;
    stateToAdd.minimaxValue = (newTurnIndex == 0) ? game.gameFoundation.negativeInfinity : game.gameFoundation.infinity;

    // other data
    stateToAdd.depth = mostRecentState.depth + 1;

    // add this copy to master list
    data.path.push_back(stateToAdd);
}

MonteCarloData MonteCarloDataGenerator::generateData(const Game &game, const GameState &initialState, const int numberOfSamples) {
    const int hardwareCores = std::thread::hardware_concurrency();

    MonteCarloData result;
    result.path.push_back(initialState);

    Game workGame = game; // need a copy so we can update possible tiles

    while (true) {
        const GameState& mostRecentState = result.path.back();

        if (mostRecentState.isLeafNode()) {
            break;
        }

        const bool isMaxPlayer = mostRecentState.isMaxPlayer();
        const int opponentHandSize = isMaxPlayer ? (int)mostRecentState.minPlayerHand.size() : (int)mostRecentState.maxPlayerHand.size();
        const DominoPointerVector& possibleTiles = isMaxPlayer ? workGame.possibleMinPlayerTiles : workGame.possibleMaxPlayerTiles;

        IntVectorVector indexCombos = CombinationListCreator().listOfCombinationIndices((int)possibleTiles.size(), opponentHandSize);
        const unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(std::begin(indexCombos), std::end(indexCombos), std::default_random_engine(seed));

        const int samplesToUse = std::min(numberOfSamples, (int)indexCombos.size());

        // sample random hands to find best move
        const int cores = samplesToUse > hardwareCores ? hardwareCores : 1;
        const int segmentSize = samplesToUse / cores;
        IntVectorVector childCountsByCore;
        std::vector<GameState> workStatesByCore;
        for (int i = 0; i < cores; i++) {
            childCountsByCore.push_back(IntVector(game.gameFoundation.handSize, 0));
            workStatesByCore.push_back(GameState::stateForMonteCarloSampling(mostRecentState));
        }
        std::vector<std::thread> threads;
        for (int i = 0; i < cores; i++) {
            const int startIndex = i * segmentSize;
            const int endIndex = (i == cores - 1) ? samplesToUse : std::min(startIndex + segmentSize, samplesToUse);
            threads.push_back(std::thread(_processSamples, std::ref(workStatesByCore[i]), std::ref(childCountsByCore[i]), startIndex, endIndex, std::ref(indexCombos), isMaxPlayer, opponentHandSize, std::ref(possibleTiles), std::ref(game.gameFoundation)));
        }
        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
        IntVector childCounts(game.gameFoundation.handSize, 0);
        for (int i = 0; i < cores; i++) {
            for (int j = 0; j < game.gameFoundation.handSize; j++) {
                childCounts[j] = childCounts[j] + childCountsByCore[i][j];
            }
        }

        // get best move from all samples (will have highest count in childIndexCounts)
        GameState& workState = workStatesByCore[0];
        const int bestChildIndex = _getBestChildIndex(childCounts);
        const GameState bestChildRemovingChildren = GameState::stateRemovingChildren(workState.children[bestChildIndex]);

        // add state to path
        _addNewStateToPathAndUpdatePossibleTiles(workState, bestChildRemovingChildren, mostRecentState, workGame, result);
    }

    return result;
}

