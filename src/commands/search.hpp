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

/* search - Prints out information about package
 *
 * Flags:
 * -O Shows all owned files
 */

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>


enum source_type
{
    SRC_URL,
    SRC_GIT,
};

enum depend_type
{
    DEP_MAKE,
    DEP_RUN,
};


struct pkg_info
{
    std::string name;
    std::string desc;
    std::string version;
    std::vector<std::tuple<source_type, std::string, std::string>> sources;
    std::vector<std::string> depends;
    std::string bef_build;
    std::string build;
    std::string aft_install;
};



void c_search(const std::string &target, const std::vector<std::pair<char, std::string>> &flags);

std::string get_target_path(const std::string& target);
std::optional<pkg_info> get_pkg_info(const std::filesystem::path &target_path, bool print_log = true);