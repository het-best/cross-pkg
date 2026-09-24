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

#include "orphans.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>

#include "search.hpp"
#include "../messages.hpp"



void c_orphans()
{
    std::vector<std::filesystem::path> packages;

    for (const std::filesystem::path path : std::filesystem::recursive_directory_iterator("/usr/repos/"))
    {
        const std::string path_str = path.string();
        if (is_directory(path) || path_str.find("utils") != std::string::npos || path_str.find("compilers") != std::string::npos)
            continue;

        const std::optional<pkg_info> info = get_pkg_info(path.string());
        if (!info.has_value() || !get_pkg_dependents(info->name).empty())
            continue;

        packages.push_back(info->name);
    }


    // Making sure that packages are in alphabetical order
    std::ranges::sort(packages,
        [](const std::filesystem::path& a, const std::filesystem::path& b)
        {
            return a < b;
        });

    for (const std::filesystem::path path : packages)
    {
        std::cout << CYAN_COL << path.filename().string() << WHITE_COL << "\n";
    }
}
