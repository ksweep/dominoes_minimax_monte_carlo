//
//  GameFoundation.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef GameFoundation_hpp
#define GameFoundation_hpp

#include <vector>

#include "Domino.hpp"

static const int kNumberOfPlayers = 2;

static const int kDefaultHandSize = 7;
static const int kDefaultMaxDominoSuit = 6;

typedef std::vector<Domino const*> DominoPointerVector;
typedef std::vector<int> IntVector;
typedef std::vector<IntVector> IntVectorVector;
typedef std::vector<Domino> DominoVector;

struct GameFoundation {
    GameFoundation(int handSize = kDefaultHandSize, int maxDominoSuit = kDefaultMaxDominoSuit) : handSize(handSize), maxDominoSuit(maxDominoSuit) {
        numberOfPlayers = kNumberOfPlayers;
        maxPlayedDominoes = (kNumberOfPlayers * handSize) - 1;
        numberOfDominoes = ((maxDominoSuit + 1) * (maxDominoSuit + 2)) / 2;
        totalPoints = 0;
        for (int i = 0; i <= maxDominoSuit; i++) {
            for (int j = i; j <= maxDominoSuit; j++) {
                totalPoints += (i + j);
                dominoSet.push_back(Domino(std::make_pair(i, j)));
                assert(CantorPairing::pairForValue(dominoSet.back().pairingValue()) == dominoSet.back().ends);
            }
        }
        infinity = totalPoints + 1;
        negativeInfinity = -1 * infinity;
        maxDepth = handSize + (2 * (handSize - 1)) + (handSize - 2);
    }

    int handSize;
    int maxDominoSuit;
    int numberOfPlayers;
    int maxPlayedDominoes;
    int numberOfDominoes;
    int totalPoints;
    int infinity;
    int negativeInfinity;
    int maxDepth;
    DominoVector dominoSet;
};

#endif /* GameFoundation_hpp */
