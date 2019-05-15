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
    DominoPointerVector shuffledDominoes;
public:
    Game(const GameFoundation& gameFoundation);

    const GameFoundation& gameFoundation;
    DominoPointerVector maxPlayerHand;
    DominoPointerVector minPlayerHand;
    DominoPointerVector possibleMaxPlayerTiles;
    DominoPointerVector possibleMinPlayerTiles;
};

#endif /* Game_hpp */
