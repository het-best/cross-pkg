/*
    cross-pkg, source based package manager
    Copyright (C) 2026 Het Best

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 */

#pragma once


#include <string>



#define CROSS_VERSION "1.3"

#define MAIN_PATH "/var/db/cross/"
#define INSTALL_PATH "/var/db/cross/install/"
#define CACHE_PATH "/var/db/cross/cache/"
#define MIRRORS_PATH "/var/db/cross/mirrors/"

#define CFLAGS "-D_FORTIFY_SOURCE=2 -fstack-protector-strong -O2 -pipe -march=native"
#define BUILD_ARGS "CFLAGS=\"" CFLAGS "\" CXXFLAGS=\"$CFLAGS\" MAKEFLAGS=\"-j" + std::to_string(std::thread::hardware_concurrency()) + "\""

inline std::string SU_CMD = "";
inline std::string DOWN_CMD = "";