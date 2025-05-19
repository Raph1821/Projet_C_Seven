#include "StrategyLoader.hpp"

#ifdef _WIN32 // Si Windows (32 ou 64 bits)
#include <windows.h>
#else
#include <dlfcn.h>
#endif


namespace sevens {

std::shared_ptr<PlayerStrategy> StrategyLoader::loadFromLibrary(const std::string& libraryPath) {
    #ifdef _WIN32
    // Conversion de std::string en std::wstring pour LoadLibraryW
    std::wstring wideLibraryPath(libraryPath.begin(), libraryPath.end()); // Normally LoadLibrary on Windows expects a Unicode path (LPCWSTR) (const WCHAR* type), but in this case our path is a *const char string so that we need to convert it to std::wstring

    // Windows: Charger la DLL
    HMODULE handle = LoadLibrary(wideLibraryPath.c_str());
    if (!handle) {
        throw std::runtime_error("[StrategyLoader] Failed to open " + libraryPath + ": Error " + std::to_string(GetLastError()));
    }

    // Charger la fonction createStrategy
    using CreateStrategyFunc = PlayerStrategy* (*)();
    auto createFunc = reinterpret_cast<CreateStrategyFunc>(GetProcAddress(handle, "createStrategy"));
    if (!createFunc) {
        FreeLibrary(handle);
        throw std::runtime_error("[StrategyLoader] createStrategy() not found in " + libraryPath);
    }
    #else 
    // macOS/Linux: Charger la bibliothèque partagée
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
    #endif 

    // Create Strategy 
    PlayerStrategy* rawStrategy = createFunc();
    return std::shared_ptr<PlayerStrategy>(rawStrategy);
}

} // namespace sevens