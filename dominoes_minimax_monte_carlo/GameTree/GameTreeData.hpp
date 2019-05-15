//
//  GameTreeData.hpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#ifndef GameTreeData_hpp
#define GameTreeData_hpp

#include "GameFoundation.hpp"

struct GameTreeData {
    GameTreeData(const GameFoundation& gf) {
        leafNodes = 0;
        minMinimaxValue = gf.infinity;
        maxMinimaxValue = gf.negativeInfinity;
        minLeafDepth = gf.maxDepth;
        maxLeafDepth = -1;
        childrenSum = 0;
        expandedNodes = 0;
    }

    int leafNodes;
    int minMinimaxValue;
    int maxMinimaxValue;
    int minLeafDepth;
    int maxLeafDepth;
    int childrenSum;
    int expandedNodes;

    inline float getAverageBranchingFactor() const {
        return (float)childrenSum / (float)expandedNodes;
    };

    inline int getTotalNodes() const {
        return expandedNodes + leafNodes;
    }

    std::string prettyString() const {
        std::string result = "";
        result += "Leaf Nodes: " + std::to_string(leafNodes) + "\n";
        result += "Min Minimax: " + std::to_string(minMinimaxValue) + "\n";
        result += "Max Minimax: " + std::to_string(maxMinimaxValue) + "\n";
        result += "Min Leaf Depth: " + std::to_string(minLeafDepth) + "\n";
        result += "Max Leaf Depth: " + std::to_string(maxLeafDepth) + "\n";
        result += "Average Branching Factor: " + std::to_string(getAverageBranchingFactor()) + "\n";
        result += "Expanded Nodes: " + std::to_string(expandedNodes) + "\n";
        result += "Total Nodes: " + std::to_string(getTotalNodes()) + "\n";
        return result;
    }
};

#endif /* GameTreeData_hpp */
