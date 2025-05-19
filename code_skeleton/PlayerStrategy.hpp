#pragma once

#include "Generic_card_parser.hpp"
#include <vector>
#include <memory>

namespace sevens {

/**
 * Interface for player strategy implementations.
 * Students will implement this interface to create their competitive agents.
 */
class PlayerStrategy {
public:
    virtual ~PlayerStrategy() = default;
    
    // Initialize the strategy with player ID and any other setup
    virtual void initialize(uint64_t playerID) = 0;
    
    // Select a card to play from the player's hand
    // Returns index of the card in hand to play, or -1 if no playable card
    virtual int selectCardToPlay(
        const std::vector<Card>& hand,
        const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout) = 0;
        
    // Called to inform the strategy about other players' moves
    virtual void observeMove(uint64_t playerID, const Card& playedCard) = 0;
    
    // Called when a player passes their turn
    virtual void observePass(uint64_t playerID) = 0;
    
    // Get a name for this strategy (for display purposes)
    virtual std::string getName() const = 0;
};

// Type for strategy factory functions (for dynamic loading)
typedef PlayerStrategy* (*CreateStrategyFn)(); // une déclaration de type qui définit un alias de type appelé CreateStrategyFn
// PlayerStrategy* : Le type de retour de la fonction pointée est un pointeur vers un objet de type PlayerStrategy
// CreateStrategyFn est un alias pour un pointeur de fonction qui : Ne prend aucun argument. Retourne un pointeur vers un objet de type PlayerStrategy.
// Pourquoi ? Cela permet de stocker des fonctions de création de stratégie dans des variables ou des conteneurs.
// -> Très utile pour des usines (factories) de création de stratégies, ou pour sélectionner dynamiquement des stratégies.

} // namespace sevens
