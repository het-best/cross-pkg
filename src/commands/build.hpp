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
 * -C     Disable checking dependencies
 * -D     Do not download sources
 * -F     Force redownloading sources even if they are already present
 * -I ... Do not install packages or if argument is given install them to this path
 * -P     Show targets paths instead of names
 * -R     Remove standard build params
 * -S     Disable striping install binaries
 * -U     Do not remove already unpacked/compiled source
 * -V     Enables verbose output
 * -Y     Auto-yes, autoresponds yes to all messages (including continue prompts)
 */

#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "search.hpp"


bool c_build(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags);
std::optional<std::pair<std::vector<pkg_info>, std::vector<std::string>>> get_target_depends(const std::string &target, std::unordered_set<std::string>& prev_depends);