#include <iostream>
#include <string>
#include <memory>
#include "StrategyLoader.hpp"
#include "MyGameMapper.hpp"

#ifdef STATIC_BUILD 
// vérifie si la macro STATIC_BUILD a été définie avant la compilation.
// Si oui, le code entre #ifdef STATIC_BUILD et #else est inclus dans la compilation
// Sinon, le code après #else est pris en compte
#include "RandomStrategy.hpp"
#include "GreedyStrategy.hpp"
#endif

#include "PlayerStrategy.hpp"

// Multi-plateforme for libraries changement 
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// Include your framework files here...
// e.g. #include "MyGameMapper.hpp"
// #include "RandomStrategy.hpp"
// #include "GreedyStrategy.hpp"
// #include "StrategyLoader.hpp"

int main(int argc, char* argv[]) {
    // This is a minimal skeleton for demonstration purposes.
    // Students should integrate their classes or call the relevant
    // game logic from MyGameMapper (or other classes) as needed.
    
    // Arguments Verification 
    if (argc < 2) {
        std::cout << "Usage: ./sevens_game [mode] [optional libs...]\n";
        return 1;
    }
    
    std::string mode = argv[1];
    sevens::MyGameMapper mapper;

    if (mode == "internal") {
        #ifdef STATIC_BUILD
            std::cout << "[main] Internal mode: 4 built-in random strategies.\n";
            //sevens::MyGameMapper mapper;

            for (int i = 0; i < 4; ++i) {
                mapper.registerStrategy(i, std::make_shared<sevens::RandomStrategy>());
            }

            mapper.compute_and_display_game(4);
        #else
            std::cerr << "[main] Internal mode is not available without STATIC_BUILD.\n";
        #endif
    }
    else if (mode == "demo") {
        #ifdef STATIC_BUILD
            std::cout << "[main] Demo mode: Random vs Greedy..\n";
            //sevens::MyGameMapper mapper;

            mapper.registerStrategy(0, std::make_shared<sevens::RandomStrategy>());
            mapper.registerStrategy(1, std::make_shared<sevens::GreedyStrategy>());
            mapper.registerStrategy(2, std::make_shared<sevens::GreedyStrategy>());
            mapper.registerStrategy(3, std::make_shared<sevens::GreedyStrategy>());
            mapper.compute_and_display_game(4);

        #else
            std::cerr << "[main] Demo mode is not available without STATIC_BUILD.\n";
        #endif
    }
    else if (mode == "competition") {
        int numPlayers=argc-2;
        if (numPlayers < 3 || numPlayers > 7) {
            std::cerr << "[main] Competition mode requires between 3 and 7 strategy libraries.\n";
            return 1;
        }

        std::cout << "[main] Starting competition with " << (argc - 2) << " players...\n";
    
        //sevens::MyGameMapper mapper;

        for (int i = 2; i < argc; ++i) {
            std::string libPath = argv[i];
            std::cout << "[main] Loading strategy from " << libPath << "...\n";

            try {
                auto strategy = sevens::StrategyLoader::loadFromLibrary(libPath);
                if (strategy) {
                    mapper.registerStrategy(i - 2, strategy); // i - 2 car player 0 = argv[2]
                } else {
                    std::cerr << "[main] Failed to load strategy from " << libPath << "\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "[main] Error loading " << libPath << ": " << e.what() << "\n";
            }
        }

        if (mapper.getRegisteredPlayerCount() != static_cast<size_t>(numPlayers)){
            std::cerr << "[main] Exactly " << numPlayers << " strategies must be loaded. Exiting...\n";
            return 1; 
        }

        auto results = mapper.compute_and_display_game(mapper.getRegisteredPlayerCount()); //argc - 2); // Nombre de joueurs = nombre de libs
        std::cout << "[main] Competition Results:\n";

        for (const auto& result : results) {
            std::cout << "  " << mapper.getPlayerStrategies().at(result.first)->getName() << "-" << result.first << " -> Final Rank " << result.second << "\n";
        }
    }else{
        std::cerr << "[main] Unknown mode: " << mode << std::endl;
        std::cerr << "Available modes : internal, demo, competition\n";
        std::cerr << "Exiting ...\n";
        return 1;
    }
    
    return 0;
}

//#endif 