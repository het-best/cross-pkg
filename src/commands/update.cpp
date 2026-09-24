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

#include "update.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "build.hpp"
#include "search.hpp"
#include "../cmd.hpp"
#include "../defines.hpp"
#include "../messages.hpp"
#include "../split.hpp"



void c_update(const std::vector<std::pair<char, std::string>> &flags)
{
#ifndef DEV_MODE
    print_msg(MSG_DEV_MODE);

    return;
#endif

    // Checking for arguments
    bool autoyes = false;
    bool verbose = false;
    uint sleep_time = 1;

    for (const auto& [flag, arg] : flags)
    {
        switch (flag)
        {
            case 'y':
                autoyes = true;
                std::cout << FLAG_PREFIX << "Autoyes is enabled" << WHITE_COL << "\n";
                break;
            case 's':
                if (arg.empty())
                    print_msg(MSG_NO_FLAG_ARG, std::string(1, flag));
                else if (std::to_string(cross_stoi(arg)) != arg)
                    print_msg(MSG_FLAG_ARG_INT, std::string(1, flag));
                else
                    sleep_time = cross_stoi(arg);
                break;
            case 'v':
                verbose = true;
                std::cout << FLAG_PREFIX << "Verbose output is enabled" << WHITE_COL << "\n";
                break;
            default:
                print_msg(MSG_UNK_FLAG, std::string(1, flag));
        }
    }


    // Fetching versions
    print_msg(MSG_PKGS_FETCH);
    uint manual_update = 0;
    std::vector<std::tuple<pkg_info, std::string, std::string, std::string>> sources_to_upd;
    std::vector<std::string> target_versions;

    for (const std::filesystem::path path : std::filesystem::directory_iterator(INSTALL_PATH))
    {
        std::optional<pkg_info> pkg_info = get_pkg_info(path / "config.crs");
        if (!pkg_info.has_value() || (pkg_info.has_value() && pkg_info->sources.empty()) || std::get<0>(pkg_info->sources.front()) != SRC_URL)
            continue;


        // Getting version
        if (verbose)
            print_msg(MSGV_PKG_FETCH, path.filename());

        auto [url, version] = get_version(std::get<1>(pkg_info->sources.front()), pkg_info->version, sleep_time);

        if (url.empty())
            continue;

        if (url == "Unsupported provider")
            manual_update++;

        sources_to_upd.push_back({ pkg_info.value(), url, version, path.filename() });
    }

    if (sources_to_upd.empty())
    {
        print_msg(MSG_NO_INSTALLED);
        return;
    }


    // Printing out information about packages
    print_msg(MSG_PKGS_UPD, std::to_string(sources_to_upd.size() - manual_update), std::to_string(manual_update));

    for (const auto& [current_info, url, version, name] : sources_to_upd)
    {
        if (url == "Unsupported provider")
            print_msg(MSG_PKGS_MAN_UPD, name);
    }

    for (const auto& [current_info, url, version, name] : sources_to_upd)
    {
        if (url != "Unsupported provider")
            print_msg(MSG_PKGS_CAN_UPD, name, current_info.version, version);
    }


    if (!autoyes)
    {
        print_msg(MSG_CONTINUE);
        std::cin.ignore();
    }
}


std::pair<std::string, std::string> get_version(const std::string &url, std::string current_version, uint sleep_time)
{
    if (url.find("github.com") != std::string::npos)
    {
        // Getting only github.com/XXX/XXX
        const std::vector<std::string> splitted_url = split(url, "/");
        std::string base_url;

        for (uint i = 0; i < 5; i++)
        {
            base_url += splitted_url[i] + "/";
        }


        // Getting last version
        sleep(sleep_time);
        std::string version = exec_cmd_echo("curl -sI \"" + base_url + "/releases/latest\" | grep -i '^location:' | awk -F'/' '{print $NF}' | tr -d '\\r'");


        // This command can fetch releases AND tags but in my experience it works worse than curl one
        if (version == "releases\n")
            version = exec_cmd_echo("git ls-remote --tags --refs \"" + base_url + "\" | awk -F'/' '{print $NF}' | sort -V | tail -n1");

        if (version.empty())
            return {};

        version.erase(version.length() - 1);
        bool has_v = version.front() == 'v';
        version = std::string(version.begin() + version.find_first_of("0123456789"), version.end());


        // Prefer manual archives
        std::string download_url;
        const std::string possible_url = base_url + "releases/download/" + ((has_v ? "v" : "") + version) + "/" + splitted_url[4] + "-" + version + ".tar.xz";

        if (system(std::string("curl -sIL -o /dev/null -w \"%{http_code}\n\" " + possible_url + " | grep -q \"^200$\"").c_str()))
            download_url = possible_url;
        else
            download_url = base_url + "archive/refs/tags/" + (has_v ? "v" : "") + version + ".tar.gz";


        // Checking version
        if (cross_stov(version) > cross_stov(current_version))
            return std::make_pair(download_url, version);
    }
    else if (url.find("ftp.gnu.org") != std::string::npos || url.find("kernel.org") != std::string::npos)
    {
        std::vector<std::string> splitted_url = split(url, "/");
        const std::string package_name = splitted_url[splitted_url.size() - 2];
        std::string base_url;

        for (uint i = 0; i < splitted_url.size() - 1; i++)
        {
            base_url += splitted_url[i] + "/";
        }


        // Getting version
        std::string version = exec_cmd_echo("curl -s " + base_url + " | grep -oE '" + package_name +
            "-[0-9]+\\.[0-9]+(\\.[0-9]+)*' | sort -V | tail -n 1 | sed 's/" + package_name + "-//'");

        if (version.empty())
            version = exec_cmd_echo("curl -s " + base_url + " | grep -oE '" + std::string(package_name.begin(), package_name.end() - 1) +
                "-[0-9]+\\.[0-9]*' | sort -V | tail -n 1 | sed 's/" + package_name + "-//'");

        if (version.empty())
            return {};

        version.erase(version.length() - 1);
        if (cross_stov(version) < cross_stov(current_version))
            return std::make_pair(base_url + package_name + "-" + version + ".tar.gz", version);
    }
    else if (url.find("download.gnome.org") != std::string::npos)
    {
        std::vector<std::string> splitted_url = split(url, "/");
        const std::string package_name= splitted_url[4];
        std::string base_url;

        for (uint i = 0; i < 5; i++)
        {
            base_url += splitted_url[i] + "/";
        }


        // Getting version
        std::string version = exec_cmd_echo("v=$(curl -s \"" + base_url + "\" | grep -oP '(?<=href=\")[0-9]+(\\.[0-9]+)+/(?=\")' | sort -V | tail -1); curl -s \"" +
            base_url + "$v\" | grep -oP '(?<=href=\")'\"" + package_name + "\"'-[0-9.]+\\.tar\\.[a-z.]+(?=\")' | sort -V | tail -1");

        if (version.empty())
            return {};

        // Returning version
        version.erase(version.length() - 1);
        std::string shortened_version(version.begin() + version.find_last_of('-') + 1, version.end() - 7);
        shortened_version = std::string(shortened_version.begin(), shortened_version.begin() + shortened_version.find_last_of('.'));

        std::string download_url = base_url + shortened_version + "/" + version;

        if (cross_stov(version) < cross_stov(current_version))
            return std::make_pair(download_url, version);
    }
    else if (url.find("gitlab.") != std::string::npos || url.find(".freedesktop.org") != std::string::npos)
    {
        std::string base_url(url.begin(), url.end() - url.find("/-/") - 4);


        // Getting last version
        sleep(sleep_time);
        std::string version = exec_cmd_echo("curl -s \"$(cut -d/ -f1-3<<<" + base_url + ")/api/v4/projects/$(cut -d/ -f4-<<<" + base_url +
            "|sed 's:/$::;s/\\.git$//;s:/:%2F:g')/repository/tags\" | grep -oP '(?<=\"name\":\")[^\"]+' | head -1");

        if (version.empty())
            return {};


        // Returning version
        version.erase(version.length() - 1);
        const std::string repo_name = split(base_url, "/").back();

        std::string download_url = base_url + "/-/archive/" + version + "/" + repo_name + "-" + version + ".tar.gz";

        if (cross_stov(version) > cross_stov(current_version))
            return std::make_pair(download_url, version);
    }
    else
    {
        std::ofstream file("test.txt");
        file << url << std::endl;
        file.close();

        return {"Unsupported provider", ""};
    }

    return {};
}
