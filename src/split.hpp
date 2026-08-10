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
#include <vector>
#include <cstdint>



std::vector<std::string> split(const std::string& str, const std::string& delimiter);

uint cross_stoi(const std::string& str);
float cross_stof(const std::string& str);
uint64_t cross_stov(const std::string& str);

std::string round_up_str(const float& value, const uint& decimals = 3);
