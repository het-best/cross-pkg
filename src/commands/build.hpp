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

/* build - Builds and installed packages
 *
 * Flags:
 * -B     Remove standard build params
 * -P     Show targets paths instead of names
 * -Y     Auto-yes, autoresponds yes to all messages (including continue prompts)
 * -D     Disabled checking dependencies
 * -I ... Do not install packages or if argument is given install them to this path
 * -R     Force redownloading sources even if they are already present
 * -P     Do not remove already unpacked/compiled source
 * -S     Do not download sources
 * -V     Enables verbose output
 */

#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "search.hpp"


bool c_build(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags);
std::optional<std::pair<std::vector<pkg_info>, std::vector<std::string>>> get_target_depends(const std::string &target, std::unordered_set<std::string>& prev_depends);