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

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "commands/build.hpp"
#include "commands/clear.hpp"
#include "commands/help.hpp"
#include "commands/list.hpp"
#include "commands/version.hpp"
#include "cmd.hpp"
#include "defines.hpp"
#include "messages.hpp"
#include "commands/download.hpp"
#include "commands/search.hpp"
#include "commands/install.hpp"
#include "commands/orphans.hpp"
#include "commands/remove.hpp"
#include "commands/update.hpp"

#ifdef WITH_LIBCURL
#include <curl/curl.h>
#endif
 

int main(const int argc, char* argv[])
{
    // Checking available commands
    if (DOWN_CMD.empty())
    {
        if (exec_cmd("curl --help >/dev/null 2>&1"))
            DOWN_CMD = "curl";
        else if (exec_cmd("wget --help >/dev/null 2>&1"))
            DOWN_CMD = "wget";
    }

    if (getuid() != 0)
    {
        if (SU_CMD.empty())
        {
            if (exec_cmd("doas -V >/dev/null 2>&1"))
                SU_CMD = "doas";
            else if (exec_cmd("sudo --help >/dev/null 2>&1"))
                SU_CMD = "sudo";
            else if (exec_cmd("eun0 --help >/dev/null 2>&1"))
                SU_CMD = "run0";
        }
    }


    // Checking paths
    if (!std::filesystem::exists(MAIN_PATH))
        exec_cmd(SU_CMD + " mkdir " + static_cast<std::string>(MAIN_PATH));

    if (!std::filesystem::exists(INSTALL_PATH))
        exec_cmd(SU_CMD + " mkdir " + static_cast<std::string>(INSTALL_PATH));

    if (!std::filesystem::exists(MIRRORS_PATH))
        exec_cmd(SU_CMD + " mkdir " + static_cast<std::string>(MIRRORS_PATH));

    if (!std::filesystem::exists(CACHE_PATH))
    {
        exec_cmd(SU_CMD + " mkdir " + static_cast<std::string>(CACHE_PATH));
        exec_cmd(SU_CMD + " chown root:wheel " + static_cast<std::string>(CACHE_PATH));
        exec_cmd(SU_CMD + " chmod -R 750 " + static_cast<std::string>(CACHE_PATH));
    }


    // Checking for provided arguments
    if (argc < 2)
    {
        print_msg(MSG_NO_CMD);
        return 1;
    }

    std::string command = argv[1];
    std::ranges::transform(command, command.begin(), [](const unsigned char c){ return std::tolower(c); });


    // Parsing arguments and flags
    std::vector<std::string> args;
    std::vector<std::pair<char, std::string>> flags;
    int current_flag = -1;

    for (int i = 2; i < argc; i++)
    {
        const std::string current = argv[i];

        if (current.length() > 1 && current[0] == '-')
        {
            const char flag_char = std::tolower(current.back());
            flags.push_back({flag_char, ""});
            current_flag = flags.size() - 1;

            continue;
        }


        if (current_flag != -1)
        {
            if (!flags[current_flag].second.empty())
                flags[current_flag].second += " ";
            flags[current_flag].second += current;
        }
        else
            args.push_back(current);
    }



    bool result = 0;
#ifdef WITH_LIBCURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
#endif
    

    // Checking which argument was provided
    if (command == "help" || command == "h")
    {
        if (args.empty())
            c_help();
        else
            c_help(args[0]);
    }
    else if (command == "build" || command == "b")
    {
        if (args.empty())
        {
            print_msg(MSG_NO_TARGET);
            result = 1;
        }

        if (!c_build(args, flags))
        {
            print_msg(MSG_BUILD_ABORTING);
            result = 1;
        }
        else
            print_msg(MSG_BUILD_SUC);
    }
    else if (command == "clear" || command == "c")
        c_clear(flags);
    else if (command == "download" || command == "d")
    {
        if (args.empty())
        {
            print_msg(MSG_NO_TARGET);
            result = 1;
        }

        if (!c_download(args, flags))
        {
            print_msg(MSG_DOWN_ABORTING);
            result = 1;
        }
    }
    else if (command == "install" || command == "i")
    {
        if (args.empty())
        {
            print_msg(MSG_NO_TARGET);
            result = 1;
        }
        else
            c_install(args, flags);
    }
    else if (command == "list" || command == "l")
        c_list();
    else if (command == "orphans" || command == "o")
        c_orphans();
    else if (command == "remove" || command == "r")
    {
        if (args.empty())
        {
            print_msg(MSG_NO_TARGET);
            result = 1;
        }
        else
            c_remove(args, flags);
    }
    else if (command == "search" || command == "s")
    {
        if (args.empty())
        {
            print_msg(MSG_NO_TARGET);
            result = 1;
        }
        else
            c_search(args[0], flags);
    }
    else if (command == "update" || command == "u")
        c_update(flags);
    else if (command == "version" || command == "v")
        c_version(flags);
#ifdef DEV_MODE
    else if (command == "test")
    {
        std::cout << WHITE_COL << "██" << WHITE_COL;
        std::cout << RED_COL << "██" << WHITE_COL;
        std::cout << GREEN_COL << "██" << WHITE_COL;
        std::cout << BLUE_COL << "██" << WHITE_COL;
        std::cout << ORANGE_COL << "██" << WHITE_COL;
        std::cout << MAG_COL << "██" << WHITE_COL;
        std::cout << CYAN_COL << "██" << WHITE_COL;
        std::cout << PINK_COL << "██" << WHITE_COL << "\n";
        
        for (uint i = 0; i < 1000; i++)
        {
            print_msg(static_cast<msg_type>(i), "{ARG1}", "{ARG2}", "{ARG3}");
            if (i % 99 == 0)
                std::cout << "\n";
        }
    }
#endif
    else
        print_msg(MSG_UNK_CMD, command);


#ifdef WITH_LIBCURL
    curl_global_cleanup();
#endif

    return result;
}
