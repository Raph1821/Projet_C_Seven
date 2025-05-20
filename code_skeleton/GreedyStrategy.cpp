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
    /*[[maybe_unused]]*/ const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout)
{
    // A trivial "greedy" approach:
    // 1. If the hand is empty, pass (-1).
    // 2. Otherwise, just pick the first card in the hand. 
    //    (We do not check adjacency or any scoring.)

    if (hand.empty()) {
        return -1; // pass
    }

    int bestIndex=-1;
    uint64_t highestRank=0;

    for (size_t i = 0; i < hand.size(); ++i) {
        const Card& card = hand[i];
        uint64_t suit = card.suit;
        uint64_t rank = card.rank;

        bool isPlayable = false;
        if (rank == 6 && (tableLayout.find(suit) == tableLayout.end() ||
                          tableLayout.at(suit).find(6) == tableLayout.at(suit).end())) {
            isPlayable = true;
        } else if (tableLayout.find(suit) != tableLayout.end()) {
            const auto& suitLayout = tableLayout.at(suit);
            if ((rank > 0 && suitLayout.find(rank - 1) != suitLayout.end()) ||
                (rank < 12 && suitLayout.find(rank + 1) != suitLayout.end())) {
                isPlayable = true;
            }
        }

        if (isPlayable && rank > highestRank) {
            highestRank = rank;
            bestIndex = static_cast<int>(i);
        }
    }

    return bestIndex;
    
    //return 0; // Always choose the first card in the hand
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

#ifdef BUILD_SHARED_LIB
extern "C" sevens::PlayerStrategy* createStrategy() {
    return new sevens::GreedyStrategy();
}
#endif

} // namespace sevens
