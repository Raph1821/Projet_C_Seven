#pragma once

#include "PlayerStrategy.hpp"
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>

// Platform-specific includes 
#ifdef _WIN32 // If Windows (32 ou 64 bits) (DLL)
#include <windows.h>
#else
#include <dlfcn.h> // If Linux/macOS (Shared Library)
#endif

namespace sevens {

/**
 * Utility class for loading player strategies from shared libraries.
 */
class StrategyLoader {
public:
    /**
     * Loads a player strategy from a shared library (DLL on Windows, .so on Linux/macOS).
     * @param libraryPath The path to the shared library.
     * @return A shared pointer to the loaded PlayerStrategy.
     * @throws std::runtime_error if the library or the strategy function cannot be loaded.
     */
    static std::shared_ptr<PlayerStrategy> loadFromLibrary(const std::string& libraryPath);
};

} // namespace sevens
