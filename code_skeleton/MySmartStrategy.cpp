#pragma once
#include "PlayerStrategy.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <unordered_map>
#include <set>

namespace sevens {

class MySmartStrategy : public PlayerStrategy {
public:
    MySmartStrategy() {
        auto seed = static_cast<unsigned long>(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
        rng.seed(seed);
    }

    ~MySmartStrategy() override = default;

    void initialize(uint64_t playerID) override {
        myID = playerID;
    }

    int selectCardToPlay(
        const std::vector<Card>& hand,
        const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout) override 
    {
        std::vector<int> playableIndices;
        std::vector<int> sevensIndices;
        std::vector<int> deferredSixesEights;
        std::vector<int> bluffable;

        for (int i = 0; i < hand.size(); ++i) {
            const Card& card = hand[i];
            uint64_t suit = card.suit;
            uint64_t rank = card.rank;

            if (rank == 7 && tableLayout.at(suit).count(rank) == 0) {
                sevensIndices.push_back(i);
                continue;
            }

            bool left = (rank > 1 && tableLayout.at(suit).count(rank - 1) && tableLayout.at(suit).at(rank - 1));
            bool right = (rank < 13 && tableLayout.at(suit).count(rank + 1) && tableLayout.at(suit).at(rank + 1));

            if (left || right) {
                // Rétention stratégique : ne pas jouer un 6 si on a le 5, ou un 8 si on a le 9
                if ((rank == 6 && hasCard(hand, suit, 5)) || (rank == 8 && hasCard(hand, suit, 9))) {
                    bluffable.push_back(i);
                } else if (rank == 6 || rank == 8) {
                    deferredSixesEights.push_back(i);
                } else {
                    playableIndices.push_back(i);
                }
            }
        }

        if (!sevensIndices.empty()) {
            std::uniform_int_distribution<int> dist(0, sevensIndices.size() - 1);
            return sevensIndices[dist(rng)];
        }

        int bestIndex = -1;
        int maxFutureUnlocks = -1;

        auto computeUnlocks = [&](int idx) {
            const Card& card = hand[idx];
            uint64_t suit = card.suit;
            uint64_t rank = card.rank;
            int futureUnlocks = 0;
            for (const Card& other : hand) {
                if (other.suit != suit || other.rank == rank) continue;
                if ((other.rank == rank + 1 && rank < 13) || (other.rank == rank - 1 && rank > 1)) {
                    futureUnlocks++;
                }
            }
            return futureUnlocks;
        };

        for (int idx : playableIndices) {
            int unlocks = computeUnlocks(idx);
            if (unlocks > maxFutureUnlocks) {
                bestIndex = idx;
                maxFutureUnlocks = unlocks;
            }
        }

        if (bestIndex != -1) return bestIndex;

        // Jouer une carte "bluffable" si possible
        if (!bluffable.empty()) {
            std::uniform_int_distribution<int> dist(0, bluffable.size() - 1);
            return bluffable[dist(rng)];
        }

        if (!deferredSixesEights.empty()) {
            std::uniform_int_distribution<int> dist(0, deferredSixesEights.size() - 1);
            return deferredSixesEights[dist(rng)];
        }

        if (!playableIndices.empty()) {
            std::uniform_int_distribution<int> dist(0, playableIndices.size() - 1);
            return playableIndices[dist(rng)];
        }

        return -1;
    }

    void observeMove(uint64_t playerID, const Card& playedCard) override {
        playedCards.insert(playedCard);
    }

    void observePass(uint64_t playerID) override {
        if (playerID != myID) {
            passCount[playerID]++;
        }
    }

    std::string getName() const override {
        return "MySmartStrategy";
    }

private:
    uint64_t myID;
    std::mt19937 rng;
    std::set<Card> playedCards;
    std::unordered_map<uint64_t, int> passCount;

    bool hasCard(const std::vector<Card>& hand, uint64_t suit, uint64_t rank) {
        for (const Card& card : hand) {
            if (card.suit == suit && card.rank == rank) return true;
        }
        return false;
    }
};

// extern "C" PlayerStrategy* createStrategy() {
//     return new MySmartStrategy();
// }

#ifdef BUILD_SHARED_LIB
extern "C" sevens::PlayerStrategy* createStrategy() {
    return new sevens::MySmartStrategy();
}
#endif

} // namespace sevens