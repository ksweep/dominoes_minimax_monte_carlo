//
//  CombinationListCreator.cpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#include <numeric>
#include <chrono>
#include <random>

#include "CombinationListCreator.hpp"

void CombinationListCreator::_listOfCombinationIndicesUtility(IntVectorVector& result, IntVector& temp, const int n, const int left, const int k) {
    if (k == 0) {
        result.push_back(temp);
        return;
    }

    for (int i = left; i <= n; i++) {
        temp.push_back(i - 1); // subtract 1 for 0-based
        _listOfCombinationIndicesUtility(result, temp, n, i + 1, k - 1);
        temp.pop_back();
    }
}

IntVectorVector CombinationListCreator::listOfCombinationIndices(const int n, const int k) {
    IntVectorVector result;
    IntVector temp;
    _listOfCombinationIndicesUtility(result, temp, n, 1, k);
    return result;
}

int CombinationListCreator::_nChooseK(const int n, int k) {
    if (n > n - k) {
        k = n - k;
    }
    int result = 1;
    for (int i = 1; i <= k; i++) {
        result *= (n + 1 - i);
        result /= i;
    }
    return result;
}

IntVector CombinationListCreator::indexList(const int n, const int k, const bool shuffle) {
    const int listSize = _nChooseK(n, k);
    std::vector<int> result(listSize);
    std::iota(result.begin(), result.end(), 0);
    if (shuffle) {
        const unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count(); // time-based seed
        std::shuffle(std::begin(result), std::end(result), std::default_random_engine(seed));
    }
    return result;
}
