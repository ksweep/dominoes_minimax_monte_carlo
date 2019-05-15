//
//  MonteCarloDataGenerator.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef MonteCarloDataGenerator_hpp
#define MonteCarloDataGenerator_hpp

#include "Game.hpp"
#include "GameState.hpp"
#include "MonteCarloData.hpp"

typedef std::vector<IntVector> IntVectorVector;

class MonteCarloDataGenerator {
private:
    static void _updateChildCounts(const bool isMaxPlayer, const GameFoundation& gameFoundation, GameState& workState, IntVector& childCounts, IntPair& maxIndices);
    static int _getBestChildIndex(IntVector& childCounts);
    static void _addNewStateToPathAndUpdatePossibleTiles(const GameState& fromState, const GameState& toState, const GameState& mostRecentState, Game& game, MonteCarloData& data);
public:
    static MonteCarloData generateData(const Game& game, const GameState& initialState, const int numberOfSamples);
};

#endif /* MonteCarloDataGenerator_hpp */
