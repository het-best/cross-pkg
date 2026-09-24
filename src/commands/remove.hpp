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

/* remove - Removes package
 *
 * Flags:
 * -V Enables verbose output
 */

#pragma once

#include <string>
#include <vector>



void c_remove(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags);
