//
//  Game.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef Game_hpp
#define Game_hpp

#include "GameFoundation.hpp"

typedef std::vector<Domino const*> DominoPointerVector;
typedef std::vector<Domino> DominoVector;

class Game {
private:
    Game(const GameFoundation& gameFoundation); // makes random game
    Game(const GameFoundation& gameFoundation, const int maxPlayerHandNumber); // makes random game with specified max hand

    void _assignHands(const int handSize, const int numberOfDominoes, DominoPointerVector& dominoes);
public:
    static Game randomGame(const GameFoundation& gameFoundation);
    static Game gameWithStartingMaxPayerHandNumber(const GameFoundation& gameFoundation, const int maxPlayerHandNumber);

    const GameFoundation& gameFoundation;
    DominoPointerVector maxPlayerHand;
    DominoPointerVector minPlayerHand;
    DominoPointerVector possibleMaxPlayerTiles;
    DominoPointerVector possibleMinPlayerTiles;
};

#endif /* Game_hpp */
