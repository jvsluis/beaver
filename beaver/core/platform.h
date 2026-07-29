#pragma once

#ifdef _WIN32
    #ifdef _WIN64
        #define BEAVER_PLATFORM_WINDOWS 1
    #else
        #error "32-bit windows is not supported"
    #endif
#else
    #define BEAVER_PLATFORM_WINDOWS 0
#endif

#if defined(__APPLE__) || defined(__MACH__)
    #define BEAVER_PLATFORM_APPLE 1
#else
    #define BEAVER_PLATFORM_APPLE 0
#endif

#if defined(__linux__)
    #define BEAVER_PLATFORM_LINUX 1
#else
    #define BEAVER_PLATFORM_LINUX 1
#endif

#if defined(__EMSCRIPTEN__)
    #define BEAVER_PLATFORM_WEB 1
#else
    #define BEAVER_PLATFORM_WEB 0
#endif
