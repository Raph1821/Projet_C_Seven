#include "PlayerStrategy.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <cstdint>
#include <memory> 

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

        if (hand.empty()){
            return -1;
        }

        int bestIndex = -1;
        int bestScore = -1;

        for (size_t i = 0; i < hand.size(); ++i) {
            const Card& card = hand[i];
            uint64_t suit = card.suit;
            uint64_t rank = card.rank;

            bool playable = false;
            if (rank == 6) {
                // playable = true;
                if (tableLayout.find(suit) == tableLayout.end() ||
                    tableLayout.at(suit).find(6) == tableLayout.at(suit).end()) {
                    playable = true;
                }
            } else if (tableLayout.count(suit)) {
                const auto& suitMap = tableLayout.at(suit);
                if ((rank > 0 && suitMap.count(rank - 1) && suitMap.at(rank - 1)) ||
                    (rank < 12 && suitMap.count(rank + 1) && suitMap.at(rank + 1))) {
                    playable = true;
                }
            }

            if (playable) {
                int score = static_cast<int>(rank);

                // Bonus si cette carte débloque une autre carte de notre main
                for (const Card& other : hand) {
                    if (other.suit == suit &&
                        (other.rank == rank + 1 || other.rank == rank - 1)) {
                        score += 5;
                    }
                }

                if (score > bestScore) {
                    bestScore = score;
                    bestIndex = static_cast<int>(i);
                }
            }
        }

        // S'il n'y a aucune carte jouable, retourner un index invalide sans provoquer une erreur
        // if (bestIndex == -1) {
        //     return 0; // ou std::uniform_int_distribution<int>(0, hand.size() - 1)(rng)
        // }

        return bestIndex;
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


#ifdef BUILD_SHARED_LIB
extern "C" sevens::PlayerStrategy* createStrategy() {
    return new sevens::MySmartStrategy();
}
#endif

} // namespace sevens
