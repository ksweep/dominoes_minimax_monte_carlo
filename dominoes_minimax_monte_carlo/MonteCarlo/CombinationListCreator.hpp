//
//  CombinationListCreator.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef CombinationListCreator_hpp
#define CombinationListCreator_hpp

#include <vector>

typedef std::vector<int> IntVector;
typedef std::vector<IntVector> IntVectorVector;

class CombinationListCreator {
private:
    static void _listOfCombinationIndicesUtility(IntVectorVector& result, IntVector& temp, const int n, const int left, const int k);
    static int _nChooseK(const int n, int k);
public:
    static IntVectorVector listOfCombinationIndices(const int n, const int k); // return a list of all (n choose k) combinations from 0 to n - 1
    static IntVector indexList(const int n, const int k, const bool shuffle); // return a list of size (n choose k) from 0 to (n choose k) - 1
};

#endif /* CombinationListCreator_hpp */
