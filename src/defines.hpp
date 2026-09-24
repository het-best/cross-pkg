#pragma once


#include <string>



#define CROSS_VERSION "1.0"

#define MAIN_PATH "/var/db/cross/"
#define INSTALL_PATH "/var/db/cross/install/"
#define CACHE_PATH "/var/db/cross/cache/"
#define BUILD_ARGS "CFLAGS=\"-O2 -pipe -march=native\" CXXFLAGS=\"-O2 -pipe -march=native\" MAKEFLAGS=\"-j" + std::to_string(std::thread::hardware_concurrency()) + "\""

inline std::string SU_CMD = "";
inline std::string DOWN_CMD = "";