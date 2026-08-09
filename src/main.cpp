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
#include "commands/remove.hpp"
#include "commands/update.hpp"


int main(const int argc, char* argv[])
{
    // Checking available commands
    if (getenv("CROSS_DOWNLOAD"))
    {
        if (static_cast<std::string>(getenv("CROSS_DOWNLOAD")) == "curl" && exec_cmd("curl --help >/dev/null 2>&1"))
            DOWN_CMD = "curl";
        else if (static_cast<std::string>(getenv("CROSS_DOWNLOAD")) == "wget" && exec_cmd("wget --help >/dev/null 2>&1"))
            DOWN_CMD = "wget";
    }

    if (DOWN_CMD.empty())
    {
        if (exec_cmd("curl --help >/dev/null 2>&1"))
            DOWN_CMD = "curl";
        else if (exec_cmd("wget --help >/dev/null 2>&1"))
            DOWN_CMD = "wget";
    }

    if (getenv("CROSS_SU"))
    {
        if (static_cast<std::string>(getenv("CROSS_SU")) == "sudo" && exec_cmd("which sudo >/dev/null 2>&1"))
            DOWN_CMD = "sudo";
        else if (static_cast<std::string>(getenv("CROSS_SU")) == "doas" && exec_cmd("which doas >/dev/null 2>&1"))
            DOWN_CMD = "doas";
        else if (static_cast<std::string>(getenv("CROSS_SU")) == "run0" && exec_cmd("which run0 >/dev/null 2>&1"))
            DOWN_CMD = "run0";
    }

    if (getuid() != geteuid())
    {
        if (SU_CMD.empty())
        {
            if (exec_cmd("sudo --help >/dev/null 2>&1"))
                SU_CMD = "sudo";
            else if (exec_cmd("doas --help >/dev/null 2>&1"))
                SU_CMD = "doas";
            else if (exec_cmd("eun0 --help >/dev/null 2>&1"))
                SU_CMD = "run0";
        }
    }


    // Checking paths
    if (!std::filesystem::exists(MAIN_PATH))
        exec_cmd(SU_CMD + " mkdir " + static_cast<std::string>(MAIN_PATH));

    if (!std::filesystem::exists(INSTALL_PATH))
        exec_cmd(SU_CMD + " mkdir " + static_cast<std::string>(INSTALL_PATH));

    if (!std::filesystem::exists(CACHE_PATH))
    {
        exec_cmd(SU_CMD + " mkdir " + static_cast<std::string>(CACHE_PATH));
        exec_cmd(SU_CMD + " chmod -R 777 " + static_cast<std::string>(CACHE_PATH));
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
            return 1;
        }

        if (!c_build(args, flags))
            print_msg(MSG_BUILD_ABORTING);
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
            return 1;
        }

        if (!c_download(args, flags))
        {
            print_msg(MSG_DOWN_ABORTING);
            return 1;
        }
    }
    else if (command == "install" || command == "i")
    {
        if (args.empty())
        {
            print_msg(MSG_NO_TARGET);
            return 1;
        }

        c_install(args, flags);
    }
    else if (command == "list" || command == "l")
        c_list();
    else if (command == "remove" || command == "r")
    {
        if (args.empty())
        {
            print_msg(MSG_NO_TARGET);
            return 1;
        }

        c_remove(args, flags);
    }
    else if (command == "search" || command == "s")
    {
        if (args.empty())
        {
            print_msg(MSG_NO_TARGET);
            return 1;
        }

        c_search(args[0], flags);
    }
    else if (command == "update" || command == "u")
        c_update(flags);
    else if (command == "version" || command == "v")
        c_version(flags);
    else
        print_msg(MSG_UNK_CMD, command);


    return 0;
}
