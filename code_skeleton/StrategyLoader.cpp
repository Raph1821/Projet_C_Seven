#include "StrategyLoader.hpp"

#ifdef _WIN32 // Si Windows (32 ou 64 bits)
#include <windows.h>
#else
#include <dlfcn.h>
#endif


namespace sevens {

// Define the function pointer type for the factory function
// This must match the signature of the function exported by the strategy libraries
typedef PlayerStrategy* (*CreateStrategyFunc)();

std::shared_ptr<PlayerStrategy> StrategyLoader::loadFromLibrary(const std::string& libraryPath) {

    void* handle = nullptr;
    void* proc = nullptr;

    #ifdef _WIN32
        // Load the dynamic-link library (DLL)
        handle = LoadLibraryA(libraryPath.c_str());
        if (!handle) {
            std::cerr << "Error loading library: " << libraryPath << " - " << GetLastError() << std::endl;
            throw std::runtime_error("[StrategyLoader] Failed to load library: " + libraryPath);
        }

        // Get the address of the exported function
        // The name of the function must be consistent across all strategy libraries
        // Explicitly cast FARPROC to void* to satisfy strict compiler
        proc = reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), "createStrategy")); //proc = GetProcAddress(static_cast<HMODULE>(handle), "createStrategy");
        if (!proc) {
            FreeLibrary(static_cast<HMODULE>(handle));
            std::cerr << "Error finding symbol 'createStrategy' in library: " << libraryPath << " - " << GetLastError() << std::endl;
            throw std::runtime_error("[StrategyLoader] Failed to find symbol 'createStrategy' in library: " + libraryPath);
        }

        // Cast the FARPROC to void* first, then reinterpret_cast to the function pointer type.
        // This two-step process can sometimes satisfy very strict compilers.
        auto createFunc = reinterpret_cast<CreateStrategyFunc>(proc); //(static_cast<void*>(proc)); // Two-step cast attempt
    #else 
        // macOS/Linux: Charger la bibliothèque partagée
        // Load the shared object (SO)
        handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Error loading library: " << libraryPath << " - " << dlerror() << std::endl;
            throw std::runtime_error("Failed to load library: " + libraryPath);
        }

        // Get the address of the exported function
        // The name of the function must be consistent across all strategy libraries
        proc = dlsym(handle, "createStrategy");
        if (!proc) {
            dlclose(handle);
            std::cerr << "Error finding symbol 'createStrategy' in library: " << libraryPath << " - " << dlerror() << std::endl;
            throw std::runtime_error("[StrategyLoader] Failed to find symbol 'createStrategy' in library: " + libraryPath);
        }

        // Cast the function pointer to the correct type
        // dlsym returns void*, which can be directly cast to the function pointer type
        auto createFunc = reinterpret_cast<CreateStrategyFunc>(proc);
    #endif 

//     // Create Strategy 
//     PlayerStrategy* rawStrategy = createFunc();
//     return std::shared_ptr<PlayerStrategy>(rawStrategy);
// }
        // Call the function to create an instance of the strategy
        PlayerStrategy* strategyInstance = createFunc();

        // Return a shared_ptr to manage the lifetime of the strategy instance
        // We use a custom deleter to ensure the library is unloaded when the strategy is no longer needed
        return std::shared_ptr<PlayerStrategy>(strategyInstance, [handle](PlayerStrategy* ptr) {
            // Delete the strategy instance
            delete ptr;
            // Unload the library
    #ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(handle));
    #else
            dlclose(handle);
    #endif
        });
    }

} // namespace sevens