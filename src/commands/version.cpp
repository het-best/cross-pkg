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

#include "version.hpp"

#include <iostream>

#include "update.hpp"
#include "../messages.hpp"
#include "../split.hpp"



void c_version(const std::vector<std::pair<char, std::string>> &flags)
{
    // Checking for arguments
    bool fetch_version = true;

    for (const auto& [flag, arg] : flags)
    {
        if (flag == 'f')
        {
            fetch_version = false;
            std::cout << FLAG_PREFIX << "Not fetching cross-pkg version is enabled" << WHITE_COL << "\n";
        }
        else
            print_msg(MSG_UNK_FLAG, std::to_string(flag));
    }


    print_msg(MSG_CROSS_VER);
    print_msg(MSGV_CROSS_LICENSE);


    // Checking for updates
    if (!fetch_version)
        return;

    const auto [url, version] = get_version("https://github.com/het-best/cross-pkg/", CROSS_VERSION);

    if (version.empty())
        return;

    if (cross_stov(version) > cross_stov(CROSS_VERSION))
        print_msg(MSG_CROSS_UPD, version);
}
