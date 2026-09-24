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

#include "help.hpp"

#include <iostream>
#include <ostream>

#include "../colors.hpp"
#include "../messages.hpp"



void c_help(const std::string &target)
{
    if (target.empty())
    {
        print_msg(MSG_CROSS_CMDS);

        print_msg(MSG_BUILD_INFO);
        print_msg(MSG_CLEAR_INFO);
        print_msg(MSG_DOWN_INFO);
        print_msg(MSG_INSTALL_INFO);
        print_msg(MSG_LIST_INFO);
        print_msg(MSG_REMOVE_INFO);
        print_msg(MSG_SEARCH_INFO);
        print_msg(MSG_UPDATE_INFO);
        print_msg(MSG_VERSION_INFO);
    }
    else if (target == "build")
    {
        print_msg(MSG_BUILD_INFO);
        std::cout << CYAN_COL << " -A     " << WHITE_COL << "Show targets paths instead of names\n";
        std::cout << CYAN_COL << " -Y     " << WHITE_COL << "Auto-yes, autoresponds yes to all messages (including continue prompts)\n";
        std::cout << CYAN_COL << " -D     " << WHITE_COL << "Disabled checking dependencies\n";
        std::cout << CYAN_COL << " -I ... " << WHITE_COL << "Do not install packages or if argument is given install them to this path\n";
        std::cout << CYAN_COL << " -R     " << WHITE_COL << "Force redownloading sources even if they are already present\n";
        std::cout << CYAN_COL << " -P     " << WHITE_COL << "Do not remove already unpacked/compiled source\n";
        std::cout << CYAN_COL << " -S     " << WHITE_COL << "Do not download sources\n";
        std::cout << CYAN_COL << " -V     " << WHITE_COL << "Enables verbose output\n";
    }
    else if (target == "clear")
    {
        print_msg(MSG_CLEAR_INFO);
        std::cout << CYAN_COL << " -Y     " << WHITE_COL << "Auto-yes, autoresponds yes to all messages (including continue prompts)\n";
        std::cout << CYAN_COL << " -E ... " << WHITE_COL << "Exclude packages from clearing\n";
        std::cout << CYAN_COL << " -I ... " << WHITE_COL << "Include packages to clearing\n";
        std::cout << CYAN_COL << " -V     " << WHITE_COL << "Enables verbose output\n";
    }
    else if (target == "download")
    {
        print_msg(MSG_DOWN_INFO);
        std::cout << CYAN_COL << " -R " << WHITE_COL << "Force redownloading sources even if they are already present\n";
        std::cout << CYAN_COL << " -P " << WHITE_COL << "Do not remove already unpacked/compiled source\n";
        std::cout << CYAN_COL << " -V " << WHITE_COL << "Enables verbose output\n";
    }
    else if (target == "install")
    {
        print_msg(MSG_INSTALL_INFO);
        std::cout << CYAN_COL << " -I ... " << WHITE_COL << "Do not install packages or if argument is given install them to this path\n";
        std::cout << CYAN_COL << " -V     " << WHITE_COL << "Enables verbose output\n";
    }
    else if (target == "list")
    {
        print_msg(MSG_LIST_INFO);
    }
    else if (target == "remove")
    {
        print_msg(MSG_REMOVE_INFO);
        std::cout << CYAN_COL << " -V " << WHITE_COL << "Enables verbose output\n";
    }
    else if (target == "search")
    {
        print_msg(MSG_SEARCH_INFO);
        std::cout << CYAN_COL << " -O " << WHITE_COL << "Shows all owned files\n";
    }
    else if (target == "update")
    {
        print_msg(MSG_UPDATE_INFO);
        std::cout << CYAN_COL << " -Y     " << WHITE_COL << "Auto-yes, autoresponds yes to all messages (including continue prompts)\n";
        std::cout << CYAN_COL << " -S ... " << WHITE_COL << "Fetching sleep time in seconds\n";
        std::cout << CYAN_COL << " -V     " << WHITE_COL << "Enables verbose output\n";
    }
    else if (target == "version")
    {
        print_msg(MSG_VERSION_INFO);
        std::cout << CYAN_COL << " -F " << WHITE_COL << "Do not fetch cross-pkg version\n";
    }
}
