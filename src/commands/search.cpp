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

#include "search.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../messages.hpp"
#include "../split.hpp"



void c_search(const std::string &target, const std::vector<std::pair<char, std::string>>& flags)
{
    // Checking for arguments
    bool all_owned = false;

    for (const auto& [flag, arg] : flags)
    {
        if (flag == 'o')
        {
            all_owned = true;
            std::cout << FLAG_PREFIX << "Showing all owned files is enabled" << WHITE_COL << "\n";
        }
        else
            print_msg(MSG_UNK_FLAG, std::string(1, flag));
    }

    // Getting target info
    const std::string target_path = get_target_path(target);
    if (target_path.empty())
        return;

    const std::optional<pkg_info> info = get_pkg_info(target_path);

    if (!info.has_value())
        return;

    std::cout << PREFIX << "Package name: " << CYAN_COL << target << "\n";
    std::cout << PREFIX << "Package path: " << CYAN_COL << target_path << "\n";
    std::cout << PREFIX << "Package description: " << CYAN_COL << info->desc << "\n";
    std::cout << PREFIX << "Package version: " << CYAN_COL << info->version << "\n";

    std::cout << PREFIX << "Package sources: " << CYAN_COL;
    for (const auto& [type, url, output] : info->sources)
    {
        std::string src_prefix;

        switch (type)
        {
            case SRC_URL:
                src_prefix = "url+";
                break;
            case SRC_GIT:
                src_prefix = "git+";
                break;
        }

        std::cout << src_prefix << url << (std::get<0>(info->sources.back()) == type ? WHITE_COL : ", ");
    }

    std::cout << "\n" << PREFIX << "Package dependencies: " << CYAN_COL;
    for (const std::string& depend : info->depends)
    {
        std::cout << depend << (info->depends.back() == depend ? WHITE_COL : ", ");
    }


    // Dependents
    std::string final_str;
    for (const std::filesystem::path path : std::filesystem::directory_iterator(INSTALL_PATH))
    {
        std::optional<pkg_info> pkg_info = get_pkg_info(path.string() + "/config.crs", false);
        if (!pkg_info.has_value())
            continue;

        if (std::ranges::find(pkg_info->depends, target) != pkg_info->depends.end())
            final_str += path.filename().string() + ", ";
    }

    if (final_str.empty())
        std::cout << "\n" << PREFIX << "Package dependents: " << CYAN_COL << "None" << WHITE_COL << "\n";
    else
    {
        final_str.erase(final_str.length() - 2);
        std::cout << "\n" << PREFIX << "Package dependents: " << CYAN_COL << final_str << WHITE_COL << "\n";
    }


    // Installed check
    const std::optional<pkg_info> inst_info = get_pkg_info(INSTALL_PATH + target + "/config.crs", false);

    if (!info.has_value())
    {
        std::cout << PREFIX << "Installed: " << ORANGE_COL << "No" << WHITE_COL << "\n";
        return;
    }

    std::cout << PREFIX << "Installed: " << GREEN_COL << "Yes" << WHITE_COL << "\n";


    // Owned files
    size_t owned_size = 0;

    std::ifstream man_file(INSTALL_PATH + target + "/manifest");
    if (!man_file.is_open())
        return;

    std::string line;
    final_str = "";

    uint i = 0;
    while(getline(man_file, line))
    {
        if (all_owned || i <= 5)
            final_str += line + ", ";

        owned_size += std::filesystem::file_size(line);
        i++;
    }

    if (final_str.empty())
        std::cout << PREFIX << "Owned files: " << CYAN_COL << "None" << WHITE_COL << "\n";
    else
    {
        std::string owned_size_str;

        if (owned_size < 1024)
            owned_size_str = round_up_str(owned_size) + "B";
        else if (owned_size < 1024 * 1024)
            owned_size_str = round_up_str(owned_size / 1024.0f) + "KB";
        else if (owned_size < 1024 * 1024 * 1024)
            owned_size_str = round_up_str(owned_size / 1024.0f / 1024.0f) + "MB";
        else if (owned_size < static_cast<size_t>(1024) * 1024 * 1024 * 1024)
            owned_size_str = round_up_str(owned_size / 1024.0f / 1024.0f / 1024.0f) + "GB";

        final_str.erase(final_str.length() - 2);
        if (!all_owned)
            final_str += "...";

        std::cout << PREFIX << "Owned files: " << CYAN_COL << final_str << WHITE_COL << "\n";
        std::cout << PREFIX << "Package size: " << CYAN_COL << owned_size_str << WHITE_COL << "\n";
    }
}

std::string get_target_path(const std::string& target)
{
    // Searching
    for (std::filesystem::path path : std::filesystem::recursive_directory_iterator("/usr/repos/"))
    {
        if (!is_directory(path))
            continue;

        if (std::filesystem::exists(path / (target + ".crs")))
            return path / (target + ".crs");
    }

    print_msg(MSG_PKG_NOT_FOUND, target);
    return "";
}

std::optional<pkg_info> get_pkg_info(const std::filesystem::path &target_path, bool print_log)
{
    std::string target_name = target_path.filename().string();

    if (target_name.find('.') == std::string::npos)
    {
        if (print_log)
            print_msg(MSG_PKG_CONF_NO_EXT, target_name);
        return std::nullopt;
    }
    target_name.erase(target_name.find_last_of('.'));
    if (target_name == "config")
        target_name = target_path.parent_path().filename().string();


    // Reading file
    std::ifstream file(target_path);
    if (!file.is_open())
    {
        if (print_log)
            print_msg(MSG_PKG_CONF_NOT_FOUND, target_name);
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string info_str = buffer.str();


    // Checking
    if (info_str.find("-desc-") == std::string::npos)
    {
        std::cout << target_path;
        if (print_log)
            print_msg(MSG_PKG_NO_FIELD, target_name, "desc");
        return std::nullopt;
    }

    if (info_str.find("-version-") == std::string::npos)
    {
        if (print_log)
            print_msg(MSG_PKG_NO_FIELD, target_name, "version");
        return std::nullopt;
    }

    if (info_str.find("-sources-") == std::string::npos)
    {
        if (print_log)
            print_msg(MSG_PKG_NO_FIELD, target_name, "sources");
        return std::nullopt;
    }

    if (info_str.find("-build-") == std::string::npos)
    {
        if (print_log)
            print_msg(MSG_PKG_NO_FIELD, target_name, "build");
        return std::nullopt;
    }

    std::vector<std::string> lines = split(info_str, "\n");

    for (std::string& line : lines)
    {
        if (!line.empty() && line.front() == '\t')
            line = std::string(line.begin() + 1, line.end());
    }


    // Parsing
    pkg_info info;
    info.name = target_name;

    for (uint i = 0; i < lines.size(); i++)
    {
        std::string line = lines[i];
        i++;


        if (line == "-desc-")
            info.desc = lines[i];
        else if (line == "-version-")
            info.version = lines[i];
        else if (line == "-sources-")
        {
            for(; i < lines.size(); i++)
            {
                line = lines[i];
                if (line.empty())
                    continue;

                if (line.front() == '-' && line.back() == '-')
                    break;


                // If commented
                if (line.front() == '#')
                    continue;


                if (line.find("+") == std::string::npos)
                {
                    if (print_log)
                        print_msg(MSG_PKG_NO_PREFIX, target_name);
                    return std::nullopt;
                }

                std::array parsed_sources =
                {
                    std::string(line.begin(), line.begin() + line.find("+")),
                    std::string(line.begin() + line.find("+") + 1, line.end()),
                };


                // Getting type
                source_type type;
                if (parsed_sources.front() == "url")
                    type = SRC_URL;
                else if (parsed_sources.front() == "git")
                    type = SRC_GIT;
                else
                {
                    if (print_log)
                        print_msg(MSG_PKG_UNK_PREFIX, target_name, parsed_sources.front());
                    return std::nullopt;
                }

                std::string output_file = "";

                if (parsed_sources.back().find(' ') != std::string::npos)
                {
                    std::vector<std::string> parsed_output = split(parsed_sources.back(), " ");

                    parsed_sources.back() = parsed_output.front();
                    output_file = parsed_output.back();
                }


                info.sources.push_back(std::make_tuple(type, parsed_sources.back(), output_file));
            }
            i--;
        }
        else if (line == "-depends-")
        {
            for(; i < lines.size(); i++)
            {
                line = lines[i];
                if (line.empty())
                    continue;

                if (line.front() == '-' && line.back() == '-')
                    break;


                // If commented
                if (line.front() != '#')
                    info.depends.push_back(line);
            }
            i--;
        }
        else if (line == "-before-build-" || line == "-build-" || line == "-after-install-")
        {
            // So that 3 if statements can be reduced to one
            std::string* build_str = nullptr;

            if (line == "-before-build-")
                build_str = &info.bef_build;
            else if (line == "-build-")
                build_str = &info.build;
            else
                build_str = &info.aft_install;

            if (lines[i].find("#!") == std::string::npos)
                *build_str += "#!/bin/sh -e\n";
            else
            {
                *build_str += lines[i] + "\n\n";
            }


            for(; i < lines.size(); i++)
            {
                line = lines[i];
                if (line.empty())
                {
                *build_str += "\n";
                    continue;
                }

                if (line.front() == '-' && line.back() == '-')
                    break;

                *build_str += line + "\n";
            }
            i--;
        }
    }


    return info;
}