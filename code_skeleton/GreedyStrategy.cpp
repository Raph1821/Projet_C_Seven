#include "GreedyStrategy.hpp"
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <memory> 

namespace sevens {

void GreedyStrategy::initialize(uint64_t playerID) {
    myID = playerID;
    // No special initialization for this minimal version
}

int GreedyStrategy::selectCardToPlay(
    const std::vector<Card>& hand,
    // utiliser l'attribut [[maybe_unused]] pour éviter l'avertissement de paramètre inutilisé tout en indiquant que tu prévois potentiellement de l'utiliser plus tard.
    [[maybe_unused]] const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout)
{
    // A trivial "greedy" approach:
    // 1. If the hand is empty, pass (-1).
    // 2. Otherwise, just pick the first card in the hand. 
    //    (We do not check adjacency or any scoring.)

    if (hand.empty()) {
        return -1; // pass
    }
    
    return 0; // Always choose the first card in the hand
}

void GreedyStrategy::observeMove(uint64_t /*playerID*/, const Card& /*playedCard*/) {
    // Ignored in minimal version
}

void GreedyStrategy::observePass(uint64_t /*playerID*/) {
    // Ignored in minimal version
}

std::string GreedyStrategy::getName() const {
    return "GreedyStrategy";
}
// #ifndef STATIC_BUILD
// // extern "C" sevens::PlayerStrategy* createStrategy() {
// //     return new sevens::GreedyStrategy(); 
// // }
// extern "C" std::shared_ptr<sevens::PlayerStrategy> createStrategy() {
//     return std::make_shared<sevens::GreedyStrategy>();
// }
// #endif
} // namespace sevens
