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

#include "download.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "search.hpp"
#include "../cmd.hpp"
#include "../messages.hpp"
#include "../split.hpp"



bool c_download(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags, const bool hide_flags_msg)
{
    // Checking for arguments
    bool force_download = false;
    bool preserve_src = false;
    bool verbose = false;

    for (const auto& [flag, arg] : flags)
    {
        switch (flag)
        {
            case 'f':
                force_download = true;
                if (!hide_flags_msg)
                    std::cout << FLAG_PREFIX << "Force source download is enabled" << WHITE_COL << "\n";
                break;
            case 'u':
                preserve_src = true;
                if (!hide_flags_msg)
                    std::cout << FLAG_PREFIX << "Source preservation is enabled" << WHITE_COL << "\n";
                break;
            case 'v':
                verbose = true;
                std::cout << FLAG_PREFIX << "Verbose output is enabled" << WHITE_COL << "\n";
                break;
            default:
                print_msg(MSG_UNK_FLAG, std::string(1, flag));
        }
    }


    for (uint i = 0; i < targets.size(); i++)
    {
        const std::string& target_name = targets[i];
        const std::string& target_path = get_target_path(target_name);
        const std::string& target_cache = CACHE_PATH + target_name + "/";

        if (target_path.empty())
            return false;


        // Checking cache
        if (verbose)
            print_msg(MSGV_PKG_CHECK_CACHE, target_name);

        if (!std::filesystem::exists(target_cache))
            exec_cmd("mkdir " + target_cache);
        if (std::filesystem::exists(target_cache + "install"))
            exec_cmd("rm -rf " + target_cache + "install");
        exec_cmd("mkdir " + target_cache + "install");

        if (verbose)
            exec_cmd("cp -rva " + target_path + "* " + target_cache);
        else
            exec_cmd("cp -ra " + target_path + "* " + target_cache);


        // Reading sources
        print_msg(MSG_PKG_READ_SRC, target_name);

        std::optional<pkg_info> info = get_pkg_info(target_path);
        if (!info.has_value())
            return false;


        // Downloading sources
        if (verbose)
            print_msg(MSGV_PKG_START_DOWN, target_name);

        if (preserve_src && std::filesystem::exists(target_cache + "source"))
            continue;
        if (!preserve_src && std::filesystem::exists(target_cache + "source"))
            exec_cmd("rm -rf " + target_cache + "source");
        if (!std::filesystem::exists(target_cache + "source"))
            exec_cmd("mkdir " + target_cache + "source");


        for (const auto& [prefix, source, output] : info->sources)
        {
            std::string source_name = split(source, "/").back();

            if (prefix == SRC_URL)
            {
                // Checking
                if (std::filesystem::exists(target_cache + source_name) && !force_download &&
                    exec_cmd("tar -atf " + target_cache + source_name + " > /dev/null 2>&1"))
                {
                    print_msg(MSG_PKG_ALR_DOWN, target_name, source);
                    continue;
                }

                print_msg(MSG_PKG_DOWN_SRC, target_name, source);


                // Downloading
                bool download_result = false;

                if (DOWN_CMD == "curl")
                    download_result = exec_cmd("curl -L " + source + " -o " + target_cache + source_name);
                else if (DOWN_CMD == "wget")
                    download_result = exec_cmd("wget " + source + " -P " + target_cache);

                if (download_result)
                    continue;


                // Checking mirrors
                const std::vector<std::string> splitted_source = split(source, "/");
                const std::string domain = splitted_source[2];
                const std::string no_domain_url(source.begin() + (splitted_source[0] + "//" + splitted_source[2]).length(), source.end());

                if (std::filesystem::exists(MIRRORS_PATH) && std::filesystem::exists(MIRRORS_PATH + domain))
                {
                    std::ifstream file(MIRRORS_PATH + domain);
                    std::string line;

                    while (getline(file, line))
                    {
                        const std::string url = splitted_source[0] + "//" + line + no_domain_url;
                        print_msg(MSG_PKG_DOWN_MIRROR, target_name, url);

                        if (DOWN_CMD == "curl")
                            download_result = exec_cmd("curl -L " + url + " -o " + target_cache + source_name);
                        else if (DOWN_CMD == "wget")
                            download_result = exec_cmd("wget " + url + " -P " + target_cache);

                        if (download_result)
                            goto extract;
                    }
                }

                print_msg(MSG_PKG_DOWN_FAIL, source);
                return false;
            }
            else if (prefix == SRC_GIT)
            {
                print_msg(MSG_PKG_DOWN_SRC, target_name, source);

                if (source.find("@") != std::string::npos)
                {
                    const std::vector<std::string> splitted_source = split(source, "@");

                    if (splitted_source.size() > 2)
                        print_msg(MSG_PKG_GIT_COMMIT, target_name, source);


                    if (!exec_cmd("git clone " + splitted_source.front() + " " + target_cache + "source"))
                    {
                        print_msg(MSG_PKG_DOWN_FAIL, splitted_source.front());
                        return false;
                    }

                    exec_cmd("(cd " + target_cache + "source && git checkout " + splitted_source.back() + " " + target_cache + "source)");
                }
                else
                {
                    if (!exec_cmd("git clone " + source + " " + target_cache + "source"))
                    {
                        print_msg(MSG_PKG_DOWN_FAIL, source);
                        return false;
                    }
                }
            }
        }
        extract:


        // Extracting sources
        print_msg(MSG_PKG_EXTR_SRC);

        for (const auto& [prefix, source, output] : info->sources)
        {
            const std::string source_name = split(source, "/").back();


            if (prefix == SRC_URL)
            {
                if (!exec_cmd("tar -atf " + target_cache + source_name + " > /dev/null 2>&1"))
                {
                    exec_cmd("cp " + target_cache + source_name + " " + target_cache + "source/" + output);
                    continue;
                }

                if (!std::filesystem::exists(target_cache + "source/" + output))
                    exec_cmd("mkdir " + target_cache + "source/" + output);

                if (verbose)
                    exec_cmd("tar -xvf" + target_cache + source_name + " -C" + target_cache + "source/" + output + " --strip-components=1");
                else
                    exec_cmd("tar -xf" + target_cache + source_name + " -C" + target_cache + "source/" + output + " --strip-components=1");
            }
        }
    }


    return true;
}
