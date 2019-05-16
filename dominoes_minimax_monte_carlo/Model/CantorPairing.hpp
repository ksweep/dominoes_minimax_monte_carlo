//
//  CantorPairing.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef CantorPairing_hpp
#define CantorPairing_hpp

#include <stdio.h>
#include <utility>
#include <math.h>

typedef std::pair<int, int> IntPair;

struct CantorPairing {
    inline static int valueForPair(const IntPair& p) {
        return (((p.first + p.second) * (p.first + p.second + 1)) / 2) + p.second;
    }
    inline static IntPair pairForValue(const int z) {
        const int w = floor((sqrt(8.0 * (float)z + 1.0) - 1.0) / 2.0 );
        const int t = ((w * w) + w) / 2;
        const int y = z - t;
        const int x = w - y;
        return std::make_pair(x, y);
    }
};

#endif /* CantorPairing_hpp */
