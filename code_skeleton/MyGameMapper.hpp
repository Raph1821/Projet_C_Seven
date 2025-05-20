#pragma once

#include "Generic_game_mapper.hpp"
#include "MyCardParser.hpp"
#include "PlayerStrategy.hpp"
#include <random> // la génération de nombres aléatoires modernes avec son contenu --> (des généateur pseudo-aléatoire(engines),des distributions)
#include <unordered_map> // Unordered map est une collection de paires clé-valeur, où les clés sont uniques et non ordonnées, ce qui signifie que les éléments ne sont pas triés
#include <vector> // C’est une directive pour inclure la bibliothèque standard C++ qui contient le type std::vector --> un tableau dynamique --> pouvoir changer de taille à l'exécution + facilement ajouter ou retirer .push_back(),.pop_back() + plus flexible que les tableaux int[] classiques
#include <memory> // les pointeurs intelligents (smart pointers) et d'autres outils de gestion de mémoire moderne en C++ (unique_ptr<T>,shared_ptr<T>,weak_ptr<T>,make_unique<T>,make_shared<T>)
#include <string> // Gérer des chaînes de caractères dynamiques, avec une gestion automatique de la mémoire
#include <cstdint>

namespace sevens {

/**
 * Enhanced Sevens simulation with strategy support:
 *  - Possibly internal mode or competition mode
 */
class MyGameMapper : public Generic_game_mapper {
public:
    MyGameMapper();
    ~MyGameMapper() = default;

    // Simule une partie sans affichage, renvoie la progression du jeu (sous forme de paires suit/rank)
    std::vector<std::pair<uint64_t, uint64_t>>compute_game_progress(uint64_t numPlayers) override;

    // Simule une partie avec affichage, renvoie aussi la progression du jeu
    std::vector<std::pair<uint64_t, uint64_t>>compute_and_display_game(uint64_t numPlayers) override;
    
    // Variante avec noms de joueurs (joueurs nommés au lieu d’IDs numériques)
    std::vector<std::pair<std::string, uint64_t>>compute_game_progress(const std::vector<std::string>& playerNames) override;
    std::vector<std::pair<std::string, uint64_t>>compute_and_display_game(const std::vector<std::string>& playerNames) override;

    // Required by Generic_card_parser and Generic_game_parser
    void read_cards(const std::string& filename) override; // Lecture des cartes depuis un fichier
    void read_game(const std::string& filename) override; // Lecture de la configuration initiale du plateau de jeu
    
    // Strategy management
    void registerStrategy(uint64_t playerID, std::shared_ptr<PlayerStrategy> strategy); // std::shared_ptr --> possession partagée comptage automatique des références, 'registerStrategy' permet d'associer une stratégie d'IA(ou humain) à un jouer spécifique via son ID  
    bool hasRegisteredStrategies() const;
    const std::unordered_map<uint64_t, std::shared_ptr<PlayerStrategy>>& getPlayerStrategies() const;

    // Nouvelle méthode pour obtenir le nombre de stratégies enregistrées
    size_t getRegisteredPlayerCount() const;

private:
    // You can define any data structures needed to track the game
    // E.g., player hands, table layout, random engine, etc.
    // ...

    // Stratégies assignées à chaque joueur (playerID -> stratégie)
    std::unordered_map<uint64_t,std::shared_ptr<PlayerStrategy>> playerStrategies;

    // Générateur de nombres aléatoires pour les actions aléatoires (distribution des cartes,etc)
    std::mt19937 random_engine ; // std::mt19937 est un moteur de génération de nombres pseudo-alétoire (algo Mersenne Twister version de 19937 bits de période)

    // Main de chaque joueur (playerID -> vecteur de cartes)
    // Chaque carte est représentée par un couple (suit,rank)
    std::unordered_map<uint64_t,std::vector<Card>> playerHands;

    // Résultats finaux du jeu (playerID -> range obtenu)
    std::vector<std::pair<uint64_t,uint64_t>> finalResults;

    // Mode d'affichage verbeux (utile pour debug ou affichage utilisateur)
    bool verboseMode = false ;

    // Track scores for each player
    std::unordered_map<uint64_t, uint64_t> playerScores; 
};

} // namespace sevens
