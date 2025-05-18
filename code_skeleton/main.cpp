#include <iostream>
#include <string>
#include <dlfcn.h> // Pour dlopen, dlsym
#include <memory>
#include "StrategyLoader.hpp"
#include "MyGameMapper.hpp"
#ifdef STATIC_BUILD
#include "RandomStrategy.hpp"
#include "GreedyStrategy.hpp"
#endif
#include "PlayerStrategy.hpp"

// Include your framework files here...
// e.g. #include "MyGameMapper.hpp"
// #include "RandomStrategy.hpp"
// #include "GreedyStrategy.hpp"
// #include "StrategyLoader.hpp"

int main(int argc, char* argv[]) {
    // This is a minimal skeleton for demonstration purposes.
    // Students should integrate their classes or call the relevant
    // game logic from MyGameMapper (or other classes) as needed.
    
    if (argc < 2) {
        std::cout << "Usage: ./sevens_game [mode] [optional libs...]\n";
        return 1;
    }
    
    std::string mode = argv[1];
    sevens::MyGameMapper mapper;
    if (mode == "internal") {
    #ifdef STATIC_BUILD
       std::cout << "[main] Internal mode: 4 built-in random strategies.\n";
       sevens::MyGameMapper mapper;

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
         sevens::MyGameMapper mapper;

         mapper.registerStrategy(0, std::make_shared<sevens::RandomStrategy>());
         mapper.registerStrategy(1, std::make_shared<sevens::GreedyStrategy>());

         mapper.compute_and_display_game(2);
    #else
         std::cerr << "[main] Demo mode is not available without STATIC_BUILD.\n";
    #endif
}
    else if (mode == "competition") {
        std::cout << "[main] Starting competition with " << (argc - 2) << " strategy libraries.\n";
    
    sevens::MyGameMapper mapper;

    for (int i = 2; i < argc; ++i) {
        std::string libPath = argv[i];
        std::cout << "[main] Loading: " << libPath << std::endl;

        try {
            auto strategy = sevens::StrategyLoader::loadFromLibrary(libPath);
            if (strategy) {
                mapper.registerStrategy(i - 2, strategy); // i - 2 car player 0 = argv[2]
            } else {
                std::cerr << "[main] Failed to load strategy from " << libPath << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[main] Error loading " << libPath << ": " << e.what() << std::endl;
        }
    }

    mapper.compute_and_display_game(argc - 2); // Nombre de joueurs = nombre de libs
}
    else {
        std::cerr << "[main] Unknown mode: " << mode << std::endl;
    }
    
    return 0;
}
