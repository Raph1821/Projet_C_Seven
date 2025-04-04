#pragma once // une directive préprocesseur qui permet d'éviter l'inclusion multiple d'un fichier d'en-tête(.h/.hpp)

#include <string>
#include <unordered_map> 
#include <iostream>

namespace sevens {  // Éviter les conflits de noms + Organiser le code + Rendre le code plus lisible 

/**
 * Represents a single playing card for Sevens:
 *   suit: 0..3  (0=Clubs,1=Diamonds,2=Hearts,3=Spades)
 *   rank: 1..13 (1=Ace,2=2,...,13=King)
 */
struct Card {   // Fonctionne comme une classe, la différence ici c'est que les membres de struct sont publics par défaut, types de données simples 
    int suit;
    int rank;

    friend std::ostream& operator<<(std::ostream& os, const Card& card) {
        os << "Card(suit=" << card.suit << ", rank=" << card.rank << ")";
        return os;
    }
};

/**
 * Base class for reading or generating cards.
 * Subclasses must override read_cards(...) to populate cards_hashmap.
 */
class Generic_card_parser {
public:
    virtual void read_cards(const std::string& filename) = 0;

    // Provide read-only access to the internal card map
    const std::unordered_map<uint64_t, Card>& get_cards_hashmap() const { // représente une table hachage (hash table) 
        // unordered_map : une structure de donnée qui stocke des paires clé-valeur --> accès rapide grâce à une fonction de hachage 
        // uint64_t :  une clé de type entier non signé 64 bits 
        // Card : type de la valeur associée à chaque clé --> une struct ou une classe 
        // & get_cards_hashmap() : retourne une référence vers une table de hachage stockée dans l'objet 
        return this->cards_hashmap;
    }

protected:
    // Key = card ID (0..51), Value = Card struct
    std::unordered_map<uint64_t, Card> cards_hashmap;
};

} // namespace sevens
