//
//  main.cpp
//
//  Copyright © 2019 Kevin Broom. All rights reserved.
//

#include <iostream>
#include <chrono>

#include "CombinationListCreator.hpp"
#include "Game.hpp"
#include "GameFoundation.hpp"
#include "GameState.hpp"
#include "GameTreeData.hpp"
#include "GameTreeDataGenerator.hpp"
#include "MinimaxData.hpp"
#include "MinimaxSearch.hpp"
#include "MonteCarloDataGenerator.hpp"

static const int kNumberOfGames = 1;
static const int kNumberOfSamples = 100; // max with default game settings is 21 choose 7 = 116280
static const bool kRunFullMinimax = true;
static const bool kRecordMinimaxPath = true;

int main(int argc, const char * argv[]) {
    const GameFoundation gameFoundation;

    std::vector<std::chrono::duration<double>> gameDurations;

    const auto totalStart = std::chrono::high_resolution_clock::now();

    for (int currentGameNumber = 0; currentGameNumber < kNumberOfGames; currentGameNumber++) {
        const auto gameStart = std::chrono::high_resolution_clock::now();

        const Game game(gameFoundation); // new game with hands and possible tiles set
        const GameState initialState(game);

        // *** Game Tree Data
        const GameTreeData gameTreeData = GameTreeDataGenerator::generateDataFromState(initialState);
        std::cout << "*Game Tree Data*\n" << gameTreeData.prettyString() << "\n";

        // *** Full Minimax Data
        MinimaxData fullMinimaxData;
        if (kRunFullMinimax) {
            GameState minimaxGameState(initialState);
            fullMinimaxData = MinimaxSearch::runOnState(minimaxGameState);
            std::cout << "*Full Minimax Data*\n" << fullMinimaxData.prettyString(false) << "\n";
            assert(gameTreeData.getTotalNodes() == fullMinimaxData.nodesVisited);
        }

        // *** Alpha-Beta Minimax Data
        GameState alphaBetaGameState(initialState);
        const MinimaxData alphaBetaMinimaxData = AlphaBetaMinimaxSearch::runOnState(alphaBetaGameState, kRecordMinimaxPath);
        std::cout << "*Alpha-Beta Minimax Data*\n" << alphaBetaMinimaxData.prettyString(kRecordMinimaxPath) << "\n";
        assert(!kRunFullMinimax || fullMinimaxData.minimaxValue == alphaBetaMinimaxData.minimaxValue);

        // *** Monte Carlo
        const MonteCarloData monteCarloData = MonteCarloDataGenerator::generateData(game, initialState, kNumberOfSamples);
        std::cout << "*Monte Carlo Data*\n" << monteCarloData.prettyString() << "\n";

        const auto gameFinish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> gameElapsed = gameFinish - gameStart;
        gameDurations.push_back(gameElapsed);
        std::cout << "Game " << currentGameNumber + 1 << " Elapsed Time: " << gameElapsed.count() << " s\n";
    }

    const auto totalFinish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = totalFinish - totalStart;
    std::cout << "Total Elapsed Time: " << elapsed.count() << " s\n";

    return 0;
}
