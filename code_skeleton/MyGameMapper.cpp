#include "MyGameMapper.hpp"
#include <iostream>
#include <fstream> // Permet de lire et d'écrire dans un fichier (file stream)
#include <sstream> // Permet de manipuler des chaînes de caractères comme si elles étaient des flux de données
#include <stdexcept> // Contenir des classes d'exceptions utilisées pour signaler des erreurs à l'exécution (runtime errors)
#include <chrono> // Pour initialiser le générateur avec l'heure actuelle 
#include <string>  // Gérer des chaînes de caractères dynamiques, avec une gestion automatique de la mémoire
#include <cstdint>
#include <algorithm>

namespace sevens {

MyGameMapper::MyGameMapper() {
    // TODO: Possibly seed random engine, etc.
    // Initialisation du moteur aléatoire avec le temps système 
    random_engine.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    // .seed --> un générateur pseudo-aléatoire ne crée pas de vrai hasard : il génère une suite déterministe de nombres qui semble aléatoire. Mais si on lui donne la même seed, il va toujours produire la même suite de nombres
    // std::chrono::steady_clock --> une des horloges fournies par la bibliothèque <chrono>, steady_clock est monotone : elle ne recule jamais (contrairement à system_clock). Parfait pour mesurer des intervalles de temps ou générer une graine stable.
    // .now() --> Renvoie le temps actuel donné par l’horloge (Type de retour : time_point → un "moment" dans le temps)
    // .time_since_epoch() --> Renvoie la durée écoulée depuis le "point zéro" de l’horloge (epoch, souvent 1970-01-01) (Type : duration, comme "156351242 nanoseconds", etc)
    // .count() --> Renvoie la valeur numérique brute de la durée (en nanosecondes ou autre selon l’horloge) (C’est ce nombre entier que l’on utilise pour en faire une seed.)
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void MyGameMapper::read_cards(const std::string& filename) {
    // TODO: e.g., call MyCardParser or implement logic here
    std::cout << "[MyGameMapper::read_cards] TODO: load or generate 52 cards.\n";
    try {
        MyCardParser parser;
        parser.read_cards(filename);
        std::cout << "[MyGameMapper::read_cards] Loaded " << cards_hashmap.size() << " cards.\n";
    } catch (const std::exception& e){
        std::cerr << "[MyGameMapper::read_cards] Loading error : " << e.what() << "\n";
        cards_hashmap.clear(); // Réinitialiser si erreur
    }    
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void MyGameMapper::read_game(const std::string& filename) {
    // TODO: e.g., initialize table_layout with 7s

    std::ifstream file(filename);
    if (file.is_open()){
        std::cout << "[MyGameMapper::read_game] Setting up table layout.\n";

        // Lire chaque ligne du fichier 
        std::string line;
        while (std::getline(file,line)){
            std::istringstream iss(line);// std::istringstream permet de traiter une chaine de caractère ('line' dans ce cas) comme un flux d'entrée 
            // Le but est de pouvoir "extraire" des éléments spécifiques (comme des nombres ou des mots) à partir de la ligne lue. Par exemple, si 
            // chaque ligne contient un "suit" et un "rank", on peut utiliser 'iss' pour les lire séparément.
            int suit,rank;

            // Si la ligne est valide (contient une couleur et un rang)
            if (iss >> suit >> rank){ // 'iss' : flux d'entrée à partir de la chaine de caractère 'line' - >> suit >> rank : extraire deux entiers à partir du flux 'iss'
                if (rank==6 && suit <= 3){ // 6 car les rangs vont de 0 à 12 ⇒ 7 = index 6
                    table_layout[static_cast<uint64_t>(suit)][static_cast<uint64_t>(rank)]=true; // static_cast est une forme de cast (conversion de type) en C++, utilisée pour convertir explicitement un type en un autre de manière sécurisée et contrôlée au moment de la compilation.
                }else{
                    std::cerr << "Ignored Card (rank ≠ 7) : suit = " << suit << ", rank = " << rank << std::endl;
                }
            }else{
                std::cerr << "Format error in file at line : " << line << std::endl;
            }
        }

        std::cout << "[MyGameMapper::read_game] Table layout: 7s on the table.\n";
    }else{
        std::cout << "[MyGameMapper::read_game] File not found. Reading or creating card deck..." ;
        std::cout << "[MyGameMapper::read_game] Initializing 7s...\n" ;

        this->read_cards(filename);
        for (const auto& [id, card] : cards_hashmap) { 
        // const auto& : Utilise une référence constante (évite de copier chaque élément, ce qui améliore la performance). 
        // [id, card] : Utilise la décomposition structurée pour directement capturer
            if (card.rank == 6 && card.suit <= 3) { // card.suit est de type unsigned (non signé). Comparer un type non signé avec >= 0 est toujours vrai, ce qui rend cette condition redondante.
                table_layout[card.suit][card.rank] = true;
            }
        }
    
        std::cout << "[MyGameMapper::read_game] 7 cards placed on the table from the deck." << std::endl;
    }
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


bool MyGameMapper::hasRegisteredStrategies() const {
    // TODO: Return whether any strategies have been registered
    return !playerStrategies.empty(); // vérifier si playerStrategies est non vide
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Nouvelle méthode pour obtenir le nombre de stratégies enregistrées
size_t MyGameMapper::getRegisteredPlayerCount() const {
    return playerStrategies.size();
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void MyGameMapper::registerStrategy(uint64_t playerID, std::shared_ptr<PlayerStrategy> strategy) {
    // TODO: store the strategy so we can use it during simulation
    // (void)playerID;
    // (void)strategy;
    if (!strategy) {
        std::cerr << "[MyGameMapper::registerStrategy] Error : Strategy not defined for player " << playerID << ".\n";
        return;
    }
    playerStrategies[playerID] = strategy;
    std::cout << "[MyGameMapper::registerStrategy] Registered " << strategy->getName() << "-" << playerID << " successfully.\n";
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


const std::unordered_map<uint64_t, std::shared_ptr<PlayerStrategy>>& MyGameMapper::getPlayerStrategies() const {
    return playerStrategies;
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Simule une partie sans affichage, renvoie la progression du jeu (sous forme de paires suit/rank)
std::vector<std::pair<uint64_t, uint64_t>> MyGameMapper::compute_game_progress(uint64_t numPlayers) {
    if (numPlayers < 3 || numPlayers > 7) {
        throw std::runtime_error("[MyGameMapper] Number of players must be between 3 and 7.");
    }

    finalResults.clear();
    playerHands.clear();
    playerScores.clear();

    for (uint64_t playerID = 0; playerID < numPlayers; ++playerID) {
        playerHands[playerID] = {};
        playerScores[playerID] = 0;
    }

    std::vector<Card> deck;
    for (const auto& [id, card] : cards_hashmap) {
        deck.push_back(card);
    }

    bool gameOver = false;
    while (!gameOver) {
        // Reset table and redistribute cards for new round
        table_layout.clear();
        std::shuffle(deck.begin(), deck.end(), random_engine);
        for (size_t i = 0; i < deck.size(); ++i) {
            playerHands[i % numPlayers].push_back(deck[i]);
        }
        for (const auto& [id, card] : cards_hashmap) {
            if (card.rank == 6 && card.suit <= 3) {
                table_layout[card.suit][card.rank] = true;
            }
        }

        // Simulate one round
        bool roundOver = false;
        uint64_t winnerID = 0;
        while (!roundOver) {
            for (uint64_t playerID = 0; playerID < numPlayers; ++playerID) {
                auto strategyIt = playerStrategies.find(playerID);
                if (strategyIt == playerStrategies.end()) {
                    std::cerr << "[MyGameMapper] No strategy for player " << playerID << "\n";
                    continue;
                }

                auto& strategy = strategyIt->second;
                auto& hand = playerHands[playerID];

                if (hand.empty()) {
                    roundOver = true;
                    winnerID = playerID;
                    if (verboseMode) {
                        std::cout << strategy->getName() << "-" << playerID << " finished with rank 1 in this round!\n";
                    }
                    break;
                }

                int cardIndex = strategy->selectCardToPlay(hand, table_layout);
                if (cardIndex >= 0 && static_cast<size_t>(cardIndex) < hand.size()) {
                    Card card = hand[cardIndex];
                    bool playable = (card.rank == 6 && !table_layout[card.suit].count(6)) ||
                                    (table_layout[card.suit].count(card.rank - 1) || table_layout[card.suit].count(card.rank + 1));
                    if (playable) {
                        table_layout[card.suit][card.rank] = true;
                        if (verboseMode) {
                            std::cout << strategy->getName() << "-" << playerID << " plays " << card << "\n";
                        }
                        hand.erase(hand.begin() + cardIndex);
                    } else {
                        if (verboseMode) {
                            std::cout << strategy->getName() << "-" << playerID << " passes (invalid card)\n";
                        }
                    }
                } else {
                    if (verboseMode) {
                        std::cout << strategy->getName() << "-" << playerID << " passes\n";
                    }
                }
            }
        }

        // Award points for leftover cards
        for (uint64_t playerID = 0; playerID < numPlayers; ++playerID) {
            if (playerID != winnerID) {
                playerScores[playerID] += playerHands[playerID].size();
                if (verboseMode) {
                    std::cout << playerStrategies[playerID]->getName() << "-" << playerID << " scored " << playerHands[playerID].size() << " points\n";
                }
            }
            playerHands[playerID].clear(); // Prepare for next round
        }

        // Check game over condition
        for (const auto& [playerID, score] : playerScores) {
            if (score >= 50) {
                gameOver = true;
                break;
            }
        }
    }

    // Determine final rankings
    uint64_t lastPlaceID = 0;
    for (const auto& [playerID, score] : playerScores) {
        if (score >= 50) {
            lastPlaceID = playerID;
            break;
        }
    }
    finalResults.emplace_back(lastPlaceID, numPlayers); // Last place (rank = numPlayers)

    std::vector<std::pair<uint64_t, uint64_t>> remainingPlayers;
    for (uint64_t playerID = 0; playerID < numPlayers; ++playerID) {
        if (playerID != lastPlaceID) {
            remainingPlayers.emplace_back(playerID, playerScores[playerID]);
        }
    }
    std::sort(remainingPlayers.begin(), remainingPlayers.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    for (size_t i = 0; i < remainingPlayers.size(); ++i) {
        finalResults.emplace_back(remainingPlayers[i].first, i + 1);
    }

    return finalResults;
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


std::vector<std::pair<uint64_t, uint64_t>>MyGameMapper::compute_and_display_game(uint64_t numPlayers)
{
    if (numPlayers < 3 || numPlayers > 7) {
        throw std::runtime_error("[MyGameMapper] Number of players must be between 3 and 7.");
    }
    // TODO: implement a verbose simulation
    std::cout << "[MyGameMapper::compute_and_display_game] Starting verbose Sevens with " << numPlayers << " players.\n";
    
    verboseMode=true; 
    auto results=compute_game_progress(numPlayers); // auto permet de déduire automatiquement le type de results (ici, c'est std::vector<std::pair<uint64_t, uint64_t>>)
    verboseMode=false;

    std::cout << "[MyGameMapper] Final Rankings:\n";
    // Afficher les résultats 
    for (const auto& result : results){ // parcourt chaque résultat dans le vecteur results.
        // std::cout << "Player " << result.first << " finished at rank " << result.second << ".\n";
        std::cout << "  " << playerStrategies[result.first]->getName() << "-" << result.first << " -> Rank " << result.second << "\n";
    }
    return results;
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


std::vector<std::pair<std::string, uint64_t>>MyGameMapper::compute_game_progress(const std::vector<std::string>& playerNames)
{
    if (playerNames.size() < 3 || playerNames.size() > 7) {
        throw std::runtime_error("[MyGameMapper] Number of players must be between 3 and 7.");
    }
    // Optional overload for name-based players
    //(void)playerNames;
    std::cout << "[MyGameMapper::compute_game_progress(names)] Simulating game with named players.\n";

    // Appeler la méthode existante pour les IDs 
    auto resultsByID = compute_game_progress(playerNames.size());

    // Convertir les résultats en utilisant les noms des jouueurs
    std::vector<std::pair<std::string,uint64_t>>results;
    // for (size_t i=0;i<resultsByID.size();++i){
    //     uint64_t playerID = resultsByID[i].first;
    //     uint64_t rank = resultsByID[i].second;
    //     results.emplace_back(playerNames[playerID],rank); // Ajoute une nouvelle paire (nom,id) au vecteur results 
    // }
    for (const auto& result : resultsByID) {
        results.emplace_back(playerNames[result.first], result.second);
    }
    return results;
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


std::vector<std::pair<std::string, uint64_t>>MyGameMapper::compute_and_display_game(const std::vector<std::string>& playerNames)
{
    if (playerNames.size() < 3 || playerNames.size() > 7) {
        throw std::runtime_error("[MyGameMapper] Number of players must be between 3 and 7.");
    }
    // Optional overload for name-based players
    //(void)playerNames;
    std::cout << "[MyGameMapper::compute_and_display_game(names)] Starting verbose Sevens with named players.\n";

    verboseMode=true;
    // Utiliser compute_game_progress avec les noms pour obtenir les résultats
    auto resultsByName = compute_game_progress(playerNames);
    verboseMode=false;

    // Afficher les résultats avec les noms des joueurs
    std::cout << "[MyGameMapper] Final Rankings:\n";
    for (const auto& result : resultsByName) {
        // std::cout << "Player " << result.first << " finished at rank " << result.second << ".\n";
        std::cout << "  " << result.first << " -> Rank " << result.second << "\n";
    }

    return resultsByName;
}

} // namespace sevens
