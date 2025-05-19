#pragma once // une directive de préprocesseur qui permet d'éviter l'inclusion multiple d'un fichier d'en-tête(.hpp)

#include "Generic_game_parser.hpp"
#include <string> // Gérer des chaînes de caractères dynamiques, avec une gestion automatique de la mémoire
#include <vector> // C’est une directive pour inclure la bibliothèque standard C++ qui contient le type std::vector --> un tableau dynamique --> pouvoir changer de taille à l'exécution + facilement ajouter ou retirer .push_back(),.pop_back() + plus flexible que les tableaux int[] classiques
#include <utility> // Ce header contient des outils utilitaires : pair,make_pair(),move,...
#include <cstdint>

namespace sevens {

/**
 * Interface for simulating a Sevens game:
 *   - compute_game_progress(numPlayers)
 *   - compute_and_display_game(numPlayers)
 *
 * Subclasses must implement these pure virtual methods to run the logic.
 */
class Generic_game_mapper : public Generic_game_parser {
public:
    virtual std::vector<std::pair<uint64_t, uint64_t>>compute_game_progress(uint64_t numPlayers) = 0; // std::pair --> un conteneur qui contient deux valeurs de types possiblement différents.

    virtual std::vector<std::pair<uint64_t, uint64_t>>compute_and_display_game(uint64_t numPlayers) = 0;

    // Overloads if you want name-based players
    virtual std::vector<std::pair<std::string, uint64_t>>compute_game_progress(const std::vector<std::string>& playerNames) {
    // std::string --> nom du joueur , uint_t --> un score ou un état de jeu : {"Alice",5} 
        (void)playerNames; // avoid unused-parameter warning (dit que la variable 'playerNames' n'est pas utilisée --> je sais que je ne l'utilise pas,c'est normal)
        throw("Not implemented"); // pour dire que cette fonction n'a pas encore été codée --> je t'autorise à redéfinir cette méthode dans une classe fille, mais par défaut elle ne fait rien (et te prévient si tu l'appelles par erreur)
    }

    virtual std::vector<std::pair<std::string, uint64_t>>compute_and_display_game(const std::vector<std::string>& playerNames) {
        (void)playerNames; // même logique
        throw("Not implemented"); // même logique 
    }
};

} // namespace sevens
