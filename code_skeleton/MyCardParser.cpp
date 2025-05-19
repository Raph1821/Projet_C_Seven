#include "MyCardParser.hpp"
#include <iostream>
#include <fstream> // Permet de lire et d'écrire dans un fichier (file stream)
#include <sstream> // Permet de manipuler des chaînes de caractères comme si elles étaient des flux de données
#include <string>  // Gérer des chaînes de caractères dynamiques, avec une gestion automatique de la mémoire
#include <cstdint>

namespace sevens {

void MyCardParser::read_cards(const std::string& filename) {
    // TODO: For example, create a standard 52-card deck.
    // Or read from an input file.
    std::ifstream file(filename);   // Essayer d'ouvrir le fichier 
    if (file.is_open()){    // Si le fichier existe et peut être ouvert 
        std::cout << "[MyCardParser::read_cards] Lecture du fichier" << filename << std::endl;
        std::string line;
        uint64_t card_id = 0 ; // ID unique pour chaque carte lue 

        // Lire chaque ligne du fichier 
        while (std::getline(file,line)){ // retourne un booléen (true/false)
            std::istringstream iss(line);// std::istringstream permet de traiter une chaine de caractère ('line' dans ce cas) comme un flux d'entrée 
            // Le but est de pouvoir "extraire" des éléments spécifiques (comme des nombres ou des mots) à partir de la ligne lue. Par exemple, si 
            // chaque ligne contient un "suit" et un "rank", on peut utiliser 'iss' pour les lire séparément.
            int suit,rank;

            // Si la ligne est valide (contient une couleur et un rang)
            if (iss >> suit >> rank){ // 'iss' : flux d'entrée à partir de la chaine de caractère 'line' - >> suit >> rank : extraire deux entiers à partir du flux 'iss'
                this->cards_hashmap[card_id] = Card(static_cast<uint64_t>(suit), static_cast<uint64_t>(rank));; // Card{suit,rank} crée une instance de la structure 'Card' où : 'suit' est le premier argument(couleur) et 'rank' le second (rang)
                card_id++; // Incrémenter l'ID de la carte
            }else{
                std::cerr << "Erreur de format dans le fichier à la ligne : " << line << std::endl;
            }
        }
        std::cout << "[MyCardParser::read_cards] Cartes lues avec succès à partir du fichier." << std::endl;
    }else{
        std::cout << "[MyCardParser::read_cards] Fichier introuvable, création d'un paquet standard de 52 cartes." << std::endl;

        // Si le fichier n'existe pas, créer un paquet standard de 52 cartes
        uint64_t card_id = 0; // ID unique pour chaque carte 
        for (int suit=0;suit<4;suit++){
            for (int rank=0;rank<52;rank++){
                this->cards_hashmap[card_id] = Card(static_cast<uint64_t>(suit), static_cast<uint64_t>(rank));;// Ajouter la carte au hashmap 
                card_id++;
            }
        }
        std::cout << "[MyCardParser::read_cards] Paquet standard de 52 cartes crée avec succès." << std::endl;

    }
    // Example stub:
    // std::cout << "[MyCardParser::read_cards] TODO: create the deck or parse from file.\n";
    // e.g., this->cards_hashmap[0] = Card{0, 1}; etc.
} 

} // namespace sevens
