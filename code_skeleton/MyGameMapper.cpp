#include "MyGameMapper.hpp"
#include <iostream>
#include <fstream> // Permet de lire et d'écrire dans un fichier (file stream)
#include <sstream> // Permet de manipuler des chaînes de caractères comme si elles étaient des flux de données
#include <stdexcept> // Contenir des classes d'exceptions utilisées pour signaler des erreurs à l'exécution (runtime errors)
#include <chrono> // Pour initialiser le générateur avec l'heure actuelle 
#include <string>  // Gérer des chaînes de caractères dynamiques, avec une gestion automatique de la mémoire

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
                if (rank==6 && suit <= 3 && suit >= 0){ // 6 car les rangs vont de 0 à 12 ⇒ 7 = index 6
                    table_layout[static_cast<uint64_t>(suit)][static_cast<uint64_t>(rank)]=true; // static_cast est une forme de cast (conversion de type) en C++, utilisée pour convertir explicitement un type en un autre de manière sécurisée et contrôlée au moment de la compilation.
                }else{
                    std::cerr << "Ignored Card (rank ≠ 7) : suit = " << suit << ", rank = " << rank << std::endl;
                }
            }else{
                std::cerr << "Format error in file at line : " << line << std::endl;
            }
        }

        std::cout << "[MyGameMapper::read_game] Maps 7 loaded from file." << std::endl;
    }else{
        std::cout << "[MyGameMapper::read_game] File not found. Reading or creating card deck..." << std::endl;
        
        this->read_cards(filename);
        for (const auto& [id, card] : cards_hashmap) { 
        // const auto& : Utilise une référence constante (évite de copier chaque élément, ce qui améliore la performance). 
        // [id, card] : Utilise la décomposition structurée pour directement capturer
            if (card.rank == 6 && card.suit >= 0 && card.suit <= 3) {
                table_layout[static_cast<uint64_t>(card.suit)][static_cast<uint64_t>(card.rank)] = true;
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


void MyGameMapper::registerStrategy(uint64_t playerID, std::shared_ptr<PlayerStrategy> strategy) {
    // TODO: store the strategy so we can use it during simulation
    // (void)playerID;
    // (void)strategy;
    if (!strategy) {
        std::cerr << "[MyGameMapper::registerStrategy] Error : Strategy not defined for player " << playerID << ".\n";
        return;
    }
    playerStrategies[playerID] = strategy;
    std::cout << "[MyGameMapper::registerStrategy] Strategy registered for player " << playerID << ".\n";
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Simule une partie sans affichage, renvoie la progression du jeu (sous forme de paires suit/rank)
std::vector<std::pair<uint64_t, uint64_t>>MyGameMapper::compute_game_progress(uint64_t numPlayers)
{
    // TODO: implement a quiet simulation
    // Return e.g. (playerID, finalRank) for each player
    std::cout << "[MyGameMapper::compute_game_progress] ...\n";

    // Réinitialiser les résultats
    finalResults.clear();

    // Initialiser les mains des joueurs
    playerHands.clear();
    
    for (const auto& [playerID, _] : playerStrategies) {
    playerHands[playerID] = {};  // Initialise une main vide pour chaque stratégie enregistrée
}

    // Distribuer les cartes aux joueurs (simple exemple avec des cartes séquentielles)
    for (uint64_t playerID = 0; playerID < numPlayers; ++playerID) {
        playerHands[playerID] = {}; // Chaque joueur a une main vide 
        for (uint64_t rank = 0; rank < 13; ++rank) { // 13 ranks de 0 à 12
            playerHands[playerID].emplace_back(playerID % 4, rank); // Chaque joueur a 13 cartes (suit varie) / %4 est utilisé pour répartir les "suits" parmi les joueurs de manière cyclique. on s'assure que la valeur du "suit" est toujours comprise entre 0 et 3.
        }
    }

    // 🎮 Simulation de la partie
    for (uint64_t playerID = 0; playerID < numPlayers; ++playerID) {
        auto strategyIt = playerStrategies.find(playerID); // Rechercher une stratégie pour ce joueur
        if (strategyIt != playerStrategies.end()) { // Si une stratégie est définie
            auto& strategy = strategyIt->second; // Récupérer la stratégie
            auto& hand = playerHands[playerID];  // Obtenir la main du joueur -> pointe directement sur le vecteur de cartes du joueur identifié par playerID -> playerHands[playerID] Accède au vecteur de cartes associé au joueur avec l'identifiant playerID dans la map playerHands

            uint64_t cardIndex = strategy->selectCardToPlay(hand, table_layout); // La stratégie choisit une carte
            if (cardIndex >= hand.size()) {
    std::cerr << "[ERREUR] Strategie a retourné un index invalide : " << cardIndex
              << " (taille de la main : " << hand.size() << ") pour le joueur " << playerID << std::endl;
    continue;
}
            if (cardIndex < hand.size()) {  // Vérifier si l'indice est valide 
                Card card = hand[cardIndex];// La carte sélectionnée 
                if (card.suit <= 3 && card.rank <= 12) {
                     table_layout[card.suit][card.rank] = true;
                     } else {
    std::cerr << "[ERREUR] Carte invalide jouée : suit=" << card.suit << ", rank=" << card.rank << std::endl;
}

hand.erase(hand.begin() + static_cast<int>(cardIndex));
                
            }
        }
    }

    // 📊 Collecter nombre de cartes restantes
    std::vector<std::pair<uint64_t, uint64_t>> playerRemainingCards;
    for (uint64_t playerID = 0; playerID < numPlayers; ++playerID) {
        playerRemainingCards.emplace_back(playerID, playerHands[playerID].size()); // Stocker (playerID, cartes restantes)
    }

    // 🔀 Trier par nombre de cartes restantes (ascendant)
    std::sort(playerRemainingCards.begin(), playerRemainingCards.end(), // 3 arguments : point de départ, point final du tri, une fonction lambda (fonction anonyme) utilisé pour définir la logique de tri 
        [](const auto& a, const auto& b) { // [] Définit une lambda (une fonction anonyme en ligne) -> Les crochets [] signifient qu'il n'y a pas de capture de variables extérieures 
            return a.second < b.second; // Moins de cartes = meilleur rang
        });

        // playerRemainingCards = {
        //     {0, 5},  // Joueur 0 a 5 cartes restantes
        //     {1, 3},  // Joueur 1 a 3 cartes restantes
        //     {2, 7}   // Joueur 2 a 7 cartes restantes
        // };

        // playerRemainingCards = {
        //     {1, 3},  // Joueur 1 (3 cartes) --> 1er
        //     {0, 5},  // Joueur 0 (5 cartes) --> 2ème
        //     {2, 7}   // Joueur 2 (7 cartes) --> 3ème
        // };

    // 🏆 Assigner le rang selon l'ordre trié (rang 1 = moins de cartes restantes)
    finalResults.clear();
    uint64_t rank = 1;
    for (const auto& prc : playerRemainingCards) {
        finalResults.emplace_back(prc.first, rank); // Assigner le rang au joueur
        rank++;
    }

    return finalResults;
    
    // return {};
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


std::vector<std::pair<uint64_t, uint64_t>>MyGameMapper::compute_and_display_game(uint64_t numPlayers)
{
    // TODO: implement a verbose simulation
    std::cout << "[MyGameMapper::compute_and_display_game] Starting verbose game simulation with " << numPlayers << " players.\n";
    auto results=compute_game_progress(numPlayers); // auto permet de déduire automatiquement le type de results (ici, c'est std::vector<std::pair<uint64_t, uint64_t>>)

    // Afficher les résultats 
    for (const auto& result : results){ // parcourt chaque résultat dans le vecteur results.
        std::cout << "Player " << result.first << " finished at rank " << result.second << ".\n";
    }
    return results;
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


std::vector<std::pair<std::string, uint64_t>>MyGameMapper::compute_game_progress(const std::vector<std::string>& playerNames)
{
    // Optional overload for name-based players
    //(void)playerNames;
    std::cout << "[MyGameMapper::compute_game_progress(names)] Simulating game with named players.\n";

    // Appeler la méthode existante pour les IDs 
    auto resultsByID = compute_game_progress(playerNames.size());

    // Convertir les résultats en utilisant les noms des jouueurs
    std::vector<std::pair<std::string,uint64_t>>results;
    for (size_t i=0;i<resultsByID.size();++i){
        uint64_t playerID = resultsByID[i].first;
        uint64_t rank = resultsByID[i].second;
        results.emplace_back(playerNames[playerID],rank); // Ajoute une nouvelle paire (nom,id) au vecteur results 
    }
    return results;
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


std::vector<std::pair<std::string, uint64_t>>MyGameMapper::compute_and_display_game(const std::vector<std::string>& playerNames)
{
    // Optional overload for name-based players
    //(void)playerNames;
    std::cout << "[MyGameMapper::compute_and_display_game(names)] Starting verbose simulation with named players.\n";

    // Utiliser compute_game_progress avec les noms pour obtenir les résultats
    auto resultsByName = compute_game_progress(playerNames);

    // Afficher les résultats avec les noms des joueurs
    for (const auto& result : resultsByName) {
        std::cout << "Player " << result.first << " finished at rank " << result.second << ".\n";
    }

    return resultsByName;
}

} // namespace sevens
