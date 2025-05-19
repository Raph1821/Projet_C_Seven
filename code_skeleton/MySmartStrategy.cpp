#include "PlayerStrategy.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <cstdint>

namespace sevens {

/**
 * Template for student strategy implementation.
 * Students should rename this class, implement the methods,
 * and compile as a shared library for competition.
 */
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
        // TODO: any further initialization
    }
    
    int selectCardToPlay(
        const std::vector<Card>& hand,
        const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout) override 
    {
         int chosenIndex = -1;
        int lowestRank = 100;

        for (int i = 0; i < hand.size(); ++i) {
            const Card& card = hand[i];
            uint64_t suit = card.suit;
            uint64_t rank = card.rank;

            bool left = (rank > 1 && tableLayout.at(suit).count(rank - 1) && tableLayout.at(suit).at(rank - 1));
            bool right = (rank < 13 && tableLayout.at(suit).count(rank + 1) && tableLayout.at(suit).at(rank + 1));

            if (left || right) {
                if (rank < lowestRank) {
                    lowestRank = rank;
                    chosenIndex = i;
                }
            }
        }

        return chosenIndex; // -1 si aucune carte jouable
    }
    
    void observeMove(uint64_t playerID, const Card& playedCard) override {
        // TODO: track other players' moves if you need
        (void)playerID;
        (void)playedCard;
    }
    
    void observePass(uint64_t playerID) override {
        // TODO: track passes if you need
        (void)playerID;
    }
    
    std::string getName() const override {
        // TODO: rename to something unique
        return "MySmartStrategy";
    }

private:
    uint64_t myID;
    std::mt19937 rng;
};

extern "C" PlayerStrategy* createStrategy() {
    return new MySmartStrategy();
}

} // namespace sevens
