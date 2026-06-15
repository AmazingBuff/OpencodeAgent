//
// Created by AmazingBuff on {{YYYY}}/{{MM}}/{{DD}}.
//

#ifndef PCH_H
#define PCH_H

// Standard library
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>

// Platform macros
#if defined(_WIN32) || defined(_WIN64)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#endif

#endif //PCH_H
