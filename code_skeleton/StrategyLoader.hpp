#pragma once

#include "PlayerStrategy.hpp"
#include <memory>
#include <string>
#include <dlfcn.h>
#include <stdexcept>
#include <iostream>

namespace sevens {

/**
 * Utility class for loading player strategies from shared libraries.
 */
class StrategyLoader {
public:
    static std::shared_ptr<PlayerStrategy> loadFromLibrary(const std::string& libraryPath);
};

} // namespace sevens
