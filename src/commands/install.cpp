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

#include "install.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include "../cmd.hpp"
#include "../messages.hpp"



bool c_install(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags, bool hide_flags_msg,
    std::pair<std::string, std::string> number)
{
    // Checking flags
    bool verbose = false;
    std::string install_path = "/";

    for (const auto& [flag, arg] : flags)
    {
        switch (flag)
        {
            case 'i':
                if (arg.empty())
                    print_msg(MSG_NO_FLAG_ARG, std::string(1, flag));
                else
                {
                    if (!std::filesystem::exists(arg))
                    {
                        if (!hide_flags_msg)
                            std::cout << ERR_PREFIX << "Cannot install into " << BLUE_COL << arg << WHITE_COL << ", directory does not exists\n";
                        continue;
                    }

                    if (!std::filesystem::is_directory(arg))
                    {
                        if (!hide_flags_msg)
                            std::cout << ERR_PREFIX << "Cannot install into " << BLUE_COL << arg << WHITE_COL << ", not a directory\n";
                        continue;
                    }

                    if (arg.back() == '/')
                        install_path = std::string(arg.begin(), arg.end() - 1);

                    if (!hide_flags_msg)
                        std::cout << FLAG_PREFIX << "Packages will be installed to: " << BLUE_COL << arg << WHITE_COL << "\n";
                }
                break;
            case 'v':
                verbose = true;
                if (!hide_flags_msg)
                    std::cout << FLAG_PREFIX << "Verbose output is enabled" << WHITE_COL << "\n";
                break;
            default:
                print_msg(MSG_UNK_FLAG, std::string(1, flag));
        }
    }


    for (uint i = 0; i < targets.size(); i++)
    {
        const std::string& target_name = targets[i];
        const std::string& target_cache = std::string(CACHE_PATH) + target_name + "/";


        // Checking cache
        if (verbose)
            print_msg(MSGV_PKG_CHECK_CACHE, target_name);

        if (!std::filesystem::exists(target_cache) || !std::filesystem::exists(target_cache + target_name + ".crs"))
        {
            print_msg(MSG_PKG_NO_CACHE, target_name);
            continue;
        }

        if (number.first.empty())
            print_msg(MSG_PKG_INSTALLING, target_name, std::to_string(i + 1) + "/" + std::to_string(targets.size()));
        else
            print_msg(MSG_PKG_INSTALLING, target_name, number.first + "/" + number.second);


        if (std::filesystem::is_empty(target_cache + "install"))
        {
            print_msg(MSG_PKG_NO_TO_INST);
            continue;
        }


        // Creating manifest
        print_msg(MSG_PKG_MANIFEST, target_name);

        std::ofstream file(target_cache + "manifest");

        for (const std::filesystem::path entry : std::filesystem::recursive_directory_iterator(target_cache + "install"))
        {
            if (is_directory(entry))
                continue;

            const std::string full_path = entry.string();
            const std::string relative_path(full_path.begin() + std::string(target_cache + "install").length(), full_path.end());
            file << relative_path << "\n";

            if (verbose)
                print_msg(MSGV_PKG_INST_FILE_FOUND, install_path + static_cast<std::string>(relative_path));
        }
        file.close();


        // Removing package files
        if (verbose)
            print_msg(MSGV_PKG_REM_FILES, target_name);

        std::ifstream man_file(target_cache + "manifest");
        if (man_file.is_open())
        {
            std::string line;

            while(getline(man_file, line))
            {
                if (!std::filesystem::exists(target_cache + "install" + line))
                {
                    if (verbose)
                        exec_cmd(SU_CMD + " rm -v " + install_path + line);
                    else
                        exec_cmd(SU_CMD + " rm " + install_path + line);
                }
            }
        }
        man_file.close();


        // Installing to INSTALL_PATH
        if (verbose)
            print_msg(MSGV_PKG_INST_FILES, target_name);

        std::string target_ins_path = install_path + INSTALL_PATH + target_name + "/";

        if (std::filesystem::exists(target_ins_path))
            exec_cmd(SU_CMD + " rm -rf " + target_ins_path);

        if (!std::filesystem::exists(target_ins_path))
            exec_cmd(SU_CMD + " mkdir " + target_ins_path);

        if (std::filesystem::exists(target_cache + "files"))
            exec_cmd(SU_CMD + " cp -r " + target_cache + "files " + target_ins_path);

        exec_cmd(SU_CMD + " cp " + target_cache + target_name + ".crs " + target_ins_path + "config.crs ");
        exec_cmd(SU_CMD + " cp " + target_cache  + "manifest " + target_ins_path);


        // Checking for already existing config files in /etc
        if (std::filesystem::exists(target_cache + "install/etc"))
        {
            const uint parent_path_size = std::string(target_cache + "install/etc").length();

            for (const std::filesystem::path entry : std::filesystem::recursive_directory_iterator(target_cache + "install/etc"))
            {
                if (is_directory(entry))
                    continue;

                const std::string filepath = entry.string();

                if (std::filesystem::exists(install_path + "/" + std::string(filepath.begin() + parent_path_size, filepath.end())))
                {
                    if (verbose)
                        exec_cmd(SU_CMD + "rm -v " + entry.string());
                    else
                        exec_cmd(SU_CMD + "rm " + entry.string());
                }
            }
        }


        // Copying files
        const std::string cpv_cmd = SU_CMD + " cp -rv --remove-destination ";
        const std::string cp_cmd = SU_CMD + " cp -r --remove-destination ";

        for (const std::filesystem::path entry : std::filesystem::recursive_directory_iterator(target_cache + "install"))
        {
            const std::string full_path = entry.string();
            std::string relative_path = std::filesystem::path(install_path + std::string(full_path.begin() + std::string(target_cache + "install").length(), full_path.end())).remove_filename().string();
            relative_path.erase(relative_path.length() - 1);

            if (!is_symlink(std::filesystem::path(relative_path)))
                continue;

            while (is_symlink(std::filesystem::path(relative_path)))
            {
                const std::string old_path = relative_path;

                relative_path = std::string(relative_path.begin(), relative_path.begin() + relative_path.find_last_of('/'));
                relative_path += "/" + read_symlink(std::filesystem::path(old_path)).string();
            }

            if (verbose)
                exec_cmd(cpv_cmd + full_path + " " + relative_path + "/");
            else
                exec_cmd(cp_cmd + full_path + " " + relative_path + "/ 2>/dev/null");
        }

        if (verbose)
            exec_cmd(cpv_cmd + target_cache + "install/* " + install_path + "/");
        else
            exec_cmd(cp_cmd + target_cache + "install/* " + install_path + "/ 2>/dev/null");

        if (number.first.empty())
            print_msg(MSG_PKG_INSTALLED, target_name, std::to_string(i + 1) + "/" + std::to_string(targets.size()));
        else
            print_msg(MSG_PKG_INSTALLED, target_name, number.first + "/" + number.second);
    }


    return true;
}
