#pragma once

#include "Generic_card_parser.hpp"
#include <unordered_map> // Unordered map est une collection de paires clé-valeur, où les clés sont uniques et non ordonnées, ce qui signifie que les éléments ne sont pas triés
#include <string> // Gérer des chaînes de caractères dynamiques, avec une gestion automatique de la mémoire

namespace sevens {

/**
 * Extends Generic_card_parser to handle game-state data for Sevens:
 *   - table_layout[suit][rank] = true if that rank is on the table.
 * Subclasses must override read_game(...) to set up the initial table.
 */
class Generic_game_parser : public Generic_card_parser {
public:
    virtual void read_game(const std::string& filename) = 0; // une méthode que les sous-classes vont utiliser 

    // Provide read-only access to the table layout
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& get_table_layout() const {
        return this->table_layout; // get_table_layout() est un getter qui retourne une référence constante vers une structure de donnée appelée 'table_layout' 
    }

protected:
    // For each suit -> rank -> bool (true if on table)
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>> table_layout; // Un unordered_map dont les clés sont des entiers de 64 bits 
    // (uint64_t) et les valeurs sont de type d'un unordered_map dont les clés sont des entiers de 64 bits (uint64_t) et les valeurs sont de type bool 
    // table_layout[suit][rank] = true signifie que la carte de couleur 'suit' et de rang 'rank' est présente sur la table

};

} // namespace sevens 
