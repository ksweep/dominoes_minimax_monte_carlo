//
//  GameState.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef GameState_hpp
#define GameState_hpp

#include <vector>
#include <utility>

#include "Game.hpp"

typedef std::pair<int, int> IntPair;
typedef std::vector<Domino const*> DominoPointerVector;

class GameState {
private:
    int _getPlayerPointTotal(const bool isMaxPlayer) const;
    Domino const* _getPlayerSmallestValueDomino(const bool isMaxPlayer) const;
    int _getPlayerSmallestValue(const bool isMaxPlayer) const;
    int _getPlayerSmallestSuitOfSmallestValue(const bool isMaxPlayer) const;
    void _updateLayoutEnds(Domino const* domino, const int layoutValue);
    void _generateState(const int tileIndex, const DominoPointerVector& playerHand, const int layoutValue, const bool pass);

    inline int _defaultMinimaxValueForTurnIndex(const int turnIndex) const {
        return turnIndex == 0 ? game.gameFoundation.negativeInfinity : game.gameFoundation.infinity;
    }
    inline DominoPointerVector& _currentPlayerHand() {
        return isMaxPlayer() ? maxPlayerHand : minPlayerHand;
    }

    // for not copying children
    GameState(const Game& game,
              const DominoPointerVector& maxPlayerHand,
              const DominoPointerVector& minPlayerHand,
              const DominoPointerVector& playedTiles,
              const int playerTurnIndex,
              const IntPair layoutEnds,
              const int depth,
              const int minimaxValue,
              const bool previousPlayerPassed,
              const bool jammed,
              GameState* parent);

    // for generating a child state
    GameState(const Game& game,
              GameState* gameState,
              const bool statePasses,
              const int layoutValue,
              const int tileIndex,
              const DominoPointerVector& playerHand);
    static GameState stateForGeneratingChildState(const Game& game,
                                                  GameState* gameState,
                                                  const bool statePasses,
                                                  const int layoutValue,
                                                  const int tileIndex,
                                                  const DominoPointerVector& playerHand);
    
    // for Monte-Carlo
    GameState(const Game& game,
              const DominoPointerVector& hand,
              const DominoPointerVector& playedTiles,
              const int playerTurnIndex,
              const IntPair layoutEnds,
              const int depth,
              const bool previousPlayerPassed,
              const bool jammed,
              GameState* parent);
    
public:
    static GameState stateRemovingChildren(const GameState& gameState);

    static GameState stateForMonteCarloSampling(const GameState& gameState);

    GameState(const Game& game);

    const Game& game;
    DominoPointerVector maxPlayerHand;
    DominoPointerVector minPlayerHand;
    DominoPointerVector playedTiles;
    int playerTurnIndex;
    IntPair layoutEnds;
    int depth;
    int minimaxValue;
    bool previousPlayerPassed;
    bool jammed;
    std::vector<GameState> children;
    GameState* parent;

    int getScore() const;
    void expandAndGenerateChildren();
    std::string prettyString() const;

    inline bool isMaxPlayer() const {
        return playerTurnIndex == 0;
    }
    inline bool isLeafNode() const {
        return getScore() != game.gameFoundation.infinity;
    }
};

#endif /* GameState_hpp */
