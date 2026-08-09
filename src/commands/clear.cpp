#include "clear.hpp"

#include <filesystem>
#include <iostream>
#include <unordered_set>

#include "../cmd.hpp"
#include "../messages.hpp"
#include "../split.hpp"



void c_clear(const std::vector<std::pair<char, std::string>> &flags)
{
    // Checking flags
    bool autoyes = false;
    bool verbose = false;

    std::unordered_set<std::string> exclude_pkgs;
    std::vector<std::string> include_pkgs;

    for (const auto& [flag, arg] : flags)
    {
        switch (flag)
        {
            case 'y':
                autoyes = true;
                std::cout << FLAG_PREFIX << "Autoyes is enabled" << WHITE_COL << "\n";
                break;
            case 'e':
                if (arg.empty())
                    print_msg(MSG_NO_FLAG_ARG, std::string(1, flag));
                else
                {
                    const std::vector<std::string> tmp_exclude_pkgs = split(arg, " ");
                    exclude_pkgs.insert(tmp_exclude_pkgs.begin(), tmp_exclude_pkgs.end());
                }
                break;
            case 'i':
                if (arg.empty())
                    print_msg(MSG_NO_FLAG_ARG, std::string(1, flag));
                else
                    include_pkgs = split(arg, " ");
                break;
            case 'v':
                verbose = true;
                std::cout << FLAG_PREFIX << "Verbose output is enabled" << WHITE_COL << "\n";
                break;
            default:
                print_msg(MSG_UNK_FLAG, std::string(1, flag));
        }
    }


    // Getting included packages
    if (include_pkgs.empty())
    {
        for (const std::filesystem::path entry : std::filesystem::directory_iterator(CACHE_PATH))
        {
            if (!exclude_pkgs.contains(entry.filename()))
                include_pkgs.push_back(entry.filename());
        }
    }


    // Checking if packages cache exist
    std::string include_pkgs_str;

    for (uint i = 0; i < include_pkgs.size(); i++)
    {
        const std::string pkg = include_pkgs[i];

        if (std::filesystem::exists(CACHE_PATH + pkg))
        {
            include_pkgs_str += pkg + ", ";
            continue;
        }


        print_msg(MSG_PKG_NO_CACHE, pkg);

        include_pkgs.erase(include_pkgs.begin() + i);
        i--;
    }

    if (!include_pkgs_str.empty())
        include_pkgs_str.erase(include_pkgs_str.length() - 2);
    else
    {
        print_msg(MSG_CACHE_CLEAR_NO_PKGS);
        return;
    }

    print_msg(MSG_CACHE_CLEAR_PKGS, include_pkgs_str);

    if (!autoyes)
    {
        print_msg(MSG_CONTINUE);
        std::cin.ignore();
    }


    // Deleting
    for (std::string pkg : include_pkgs)
    {
        if (verbose)
            exec_cmd(SU_CMD + " rm -rvf " + CACHE_PATH + pkg);
        else
            exec_cmd(SU_CMD + " rm -rf " + CACHE_PATH + pkg);
    }

    print_msg(MSG_CACHE_CLEARED);
}