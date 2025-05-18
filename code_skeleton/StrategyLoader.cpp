#include "StrategyLoader.hpp"

namespace sevens {

std::shared_ptr<PlayerStrategy> StrategyLoader::loadFromLibrary(const std::string& libraryPath) {
    void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
    if (!handle) {
        throw std::runtime_error("[StrategyLoader] Failed to open " + libraryPath + ": " + dlerror());
    }

    typedef PlayerStrategy* (*CreateStrategyFunc)();
    dlerror(); // Clear existing errors

    auto createFunc = reinterpret_cast<CreateStrategyFunc>(dlsym(handle, "createStrategy"));
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        throw std::runtime_error("[StrategyLoader] createStrategy() not found in " + libraryPath);
    }

    PlayerStrategy* rawStrategy = createFunc();
    return std::shared_ptr<PlayerStrategy>(rawStrategy);
}

} // namespace sevens