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

#include "list.hpp"

#include <filesystem>
#include <iostream>

#include "search.hpp"
#include "../defines.hpp"
#include "../messages.hpp"



void c_list()
{
    uint installed_count = 0;
    uint max_length = 0;

    for (const std::filesystem::path path : std::filesystem::directory_iterator(INSTALL_PATH))
    {
        max_length = std::max(max_length, static_cast<uint>(path.filename().string().length()));
    }

    for (const std::filesystem::path path : std::filesystem::directory_iterator(INSTALL_PATH))
    {
        std::optional<pkg_info> info = get_pkg_info(path.string() + "/config.crs", false);
        if (!info.has_value())
            continue;

        std::cout << path.filename().string() << std::string(max_length - path.filename().string().length() + 1, ' ') << CYAN_COL << info->version << WHITE_COL << "\n";
        installed_count++;
    }

    if (installed_count == 0)
    {
        print_msg(MSG_NO_INSTALLED);
        return;
    }

    print_msg(MSG_TOTAL_INSTALLED, std::to_string(installed_count));
}
