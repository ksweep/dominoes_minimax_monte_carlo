//
//  Domino.h
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef Domino_hpp
#define Domino_hpp

#include <utility>
#include <string>

#include "CantorPairing.hpp"

typedef std::pair<int, int> IntPair;

struct Domino {
    Domino(IntPair ends) : ends(ends) {}
    const IntPair ends;
    inline int getTileTotal() const {
        return ends.first + ends.second;
    }
    inline int smallerSuit() const {
        return ends.first < ends.second ? ends.first : ends.second;
    }
    inline bool hasSuit(const int suit) const {
        return ends.first == suit || ends.second == suit;
    }
    inline std::string prettyString() const {
        return "(" + std::to_string(ends.first) + ", " + std::to_string(ends.second) + ")";
    }
    inline int pairingValue() const {
        return CantorPairing::valueForPair(ends);
    }
};

#endif /* Domino_hpp */
