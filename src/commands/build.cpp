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

#include "build.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "download.hpp"
#include "search.hpp"
#include "install.hpp"
#include "../cmd.hpp"
#include "../colors.hpp"
#include "../defines.hpp"
#include "../messages.hpp"
#include "../split.hpp"



bool c_build(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags)
{
    // Checking flags
    bool always_show_paths = false;
    bool autoyes = false;
    bool build_params = true;
    bool depends_disable = false;
    bool dont_install = false;
    bool force_download = false;
    bool preserve_src = false;
    bool skip_download = false;
    bool skip_strip = false;
    bool verbose = false;

    std::string install_path = "";

    for (const auto& [flag, arg] : flags)
    {
        switch (flag)
        {
            case 'p':
                always_show_paths = true;
                std::cout << FLAG_PREFIX << "Always showing paths is enabled" << WHITE_COL << "\n";
                break;
            case 'y':
                autoyes = true;
                std::cout << FLAG_PREFIX << "Autoyes is enabled" << WHITE_COL << "\n";
                break;
            case 'r':
                build_params = false;
                std::cout << FLAG_PREFIX << "Adding standard build params is disabled" << WHITE_COL << "\n";
                break;
            case 'c':
                depends_disable = true;
                std::cout << FLAG_PREFIX << "Depends checking is disabled" << WHITE_COL << "\n";
                break;
            case 'i':
                if (arg.empty())
                {
                    dont_install = true;
                    std::cout << FLAG_PREFIX << "Package installation is disabled" << WHITE_COL << "\n";
                }
                else
                {
                    if (!std::filesystem::exists(arg))
                    {
                        std::cout << ERR_PREFIX << "Cannot install into " << BLUE_COL << arg << WHITE_COL << ", directory does not exists\n";
                        continue;
                    }

                    if (!std::filesystem::is_directory(arg))
                    {
                        std::cout << ERR_PREFIX << "Cannot install into " << BLUE_COL << arg << WHITE_COL << ", not a directory\n";
                        continue;
                    }

                    install_path = std::string(arg.begin(), arg.end() - 1);
                    std::cout << FLAG_PREFIX << "Packages will be installed to: " << BLUE_COL << arg << WHITE_COL << "\n";
                }
                break;
            case 'f':
                force_download = true;
                std::cout << FLAG_PREFIX << "Force source download is enabled" << WHITE_COL << "\n";
                break;
            case 'u':
                preserve_src = true;
                std::cout << FLAG_PREFIX << "Source preservation is enabled" << WHITE_COL << "\n";
                break;
            case 'd':
                skip_download = true;
                std::cout << FLAG_PREFIX << "Skipping downloading is enabled" << WHITE_COL << "\n";
                break;
            case 's':
                skip_strip = true;
                std::cout << FLAG_PREFIX << "Skipping striping is enabled" << WHITE_COL << "\n";
                break;
            case 'v':
                verbose = true;
                std::cout << FLAG_PREFIX << "Verbose output is enabled" << WHITE_COL << "\n";
                break;
            default:
                print_msg(MSG_UNK_FLAG, std::string(1, flag));
        }
    }


    // Targets infos
    std::vector<std::filesystem::path> target_paths;
    std::vector<pkg_info> target_infos;


    // Getting implicit and explicit targets
    if (verbose)
        print_msg(MSGV_GET_IMP_TARGETS);

    std::string exp_targets_str;
    std::string imp_targets_str;

    for (const std::string& target : targets)
    {
        if (!depends_disable)
        {
            // Getting all dependencies names and paths of current target
            auto names_view = target_infos | std::views::transform(&pkg_info::name);
            std::unordered_set depends_set(names_view.begin(), names_view.end());
            std::optional<std::pair<std::vector<pkg_info>, std::vector<std::string>>> depends = get_target_depends(target, depends_set);

            if (!depends.has_value())
                return false;

            if (depends->first.empty())
                continue;


            // Adding information to vectors
            target_infos.insert( target_infos.end(), depends.value().first.begin(), depends.value().first.end() );
            target_paths.insert( target_paths.end(), depends.value().second.begin(), depends.value().second.end() );
        }
        else
        {
            target_paths.push_back(get_target_path(target));

            const std::optional<pkg_info> info = get_pkg_info(target_paths.back());
            if (!info.has_value())
                return false;

            target_infos.push_back(info.value());
        }

        if (always_show_paths)
            exp_targets_str += get_target_path(target) + ", ";
        else
            exp_targets_str += target + ", ";
    }


    // Adding implicit targets to the string
    for (uint i = 0; i < target_infos.size(); i++)
    {
        const std::string& target_name = target_infos[i].name;
        const std::string& target_path = target_paths[i];


        // Checking if target is implicit
        if (std::ranges::find(targets, target_name) != targets.end())
            continue;


        if (always_show_paths)
            imp_targets_str += target_path + ", ";
        else
            imp_targets_str += target_name + ", ";
    }

    if (imp_targets_str.empty())
        imp_targets_str = "None, ";


    // Erasing ", "
    exp_targets_str.erase(exp_targets_str.length() - 2);
    imp_targets_str.erase(imp_targets_str.length() - 2);


    // Printing build info
    print_msg(MSG_BUILDING_EXP, exp_targets_str);
    print_msg(MSG_BUILDING_IMP, imp_targets_str);

    if (!autoyes)
    {
        print_msg(MSG_CONTINUE);
        std::cin.ignore();
    }


    // Checking that targets are valid
    print_msg(MSG_PKG_VALIDATION);

    for (uint i = 0; i < target_infos.size(); i++)
    {
        const pkg_info& info = target_infos[i];

        // Checking version
        std::optional<pkg_info> inst_info = get_pkg_info(install_path + INSTALL_PATH + info.name + "/config.crs", false);

        if (!inst_info.has_value())
        {
            print_msg(MSG_PKG_NEW, info.name, info.version);
            continue;
        }

        if (cross_stov(info.version) > cross_stov(inst_info->version))
            print_msg(MSG_PKG_LOWER_VER, info.name, info.version, inst_info->version);
        else if (cross_stov(info.version) == cross_stov(inst_info->version))
            print_msg(MSG_PKG_REBUILD, info.name);
        else
        {
            if (autoyes)
                continue;

            print_msg(MSG_PKG_HIGHER_VER, info.name, inst_info->version, info.version);


            // Asking user if he wants to "downgrade"
            print_msg(MSG_PKG_HIGHER_VER_QW);

            std::string input;
            std::getline(std::cin, input);

            if (input == "n" || input == "N")
            {
                target_infos.erase(target_infos.begin() + i);
                i--;
                continue;
            }

            if (input == "yy" || input == "Yy" || input == "yY" || input == "YY")
                autoyes = true;
        }
    }


    // Downloading sources
    std::vector<std::pair<char, std::string>> tmp_args;

    if (!skip_download)
    {
        if (force_download)
            tmp_args.push_back({'f', ""});
        if (preserve_src)
            tmp_args.push_back({'u', ""});
        if (verbose)
            tmp_args.push_back({'v', ""});

        auto names_view = target_infos | std::views::transform(&pkg_info::name);
        if (!c_download(std::vector(names_view.begin(), names_view.end()), tmp_args, true))
            return false;
    }


    // Building
    print_msg(MSG_BUILD_START);
    std::vector<float> target_times;

    tmp_args = {};

    if (!install_path.empty())
        tmp_args.push_back({'i', install_path + "/"});
    if (verbose)
        tmp_args.push_back({'v', ""});


    for (uint i = 0; i < target_infos.size(); i++)
    {
        const std::string& target_name = target_infos[i].name;
        const std::string& target_cache = CACHE_PATH + target_name + "/";


        // Checking if package has been installed
        const std::optional<pkg_info> inst_info = get_pkg_info(INSTALL_PATH + target_name + "/config.crs", false);
        std::chrono::time_point build_start = std::chrono::system_clock::now();


        // Before build script
        if (!target_infos[i].bef_build.empty())
        {
            std::ofstream file(target_cache + "before-build");
            file << target_infos[i].bef_build;
            file.close();

            if (build_params)
                exec_cmd("chmod +x " + target_cache + "before-build && (cd " + target_cache + "source && " + BUILD_ARGS + " ../before-build)");
            else
                exec_cmd("chmod +x " + target_cache + "before-build && (cd " + target_cache + "source && ../before-build)");
        }


        // Building
        print_msg(MSG_PKG_BUILDING, target_name, std::to_string(i + 1) + "/" + std::to_string(target_infos.size()));

        if (verbose)
            print_msg(MSGV_BUILD_ARGS, target_name, BUILD_ARGS);

        std::ofstream build_file(target_cache + "build");
        build_file << target_infos[i].build;
        build_file.close();

        bool build_result;

        if (build_params)
            build_result = exec_cmd("chmod +x " + target_cache + "build && (cd " + target_cache + "source && " + BUILD_ARGS + " ../build " + target_cache + "install)");
        else
            build_result = exec_cmd("chmod +x " + target_cache + "build && (cd " + target_cache + "source && ../build " + target_cache + "install)");

        if (!build_result)
        {
            print_msg(MSG_PKG_BUILD_FAIL, target_name);
            return false;
        }


        // Striping binaries
        if (!skip_strip)
        {
            if (verbose)
                print_msg(MSGV_BUILD_STRIP);

            exec_cmd("find " + target_cache + "install | xargs strip --strip-unneeded 2>/dev/null");
        }


        // Build time
        target_times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - build_start).count() / 1000.0f);
        print_msg(MSG_PKG_BUILD_SUC, target_name, std::to_string(i + 1) + "/" + std::to_string(target_infos.size()), round_up_str(target_times.back()));

        if (std::filesystem::exists(install_path + INSTALL_PATH + target_name + "/build-time"))
        {
            // Reading previous build time
            std::ifstream file(install_path + INSTALL_PATH + target_name + "/build-time");
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            
            std::string prev_time = buffer.str();
            if (prev_time.back() == '\n')
                prev_time.erase(prev_time.length() - 1);

            const float prev_time_f = std::stof(prev_time);

            if (prev_time_f < target_times.back())
                print_msg(MSG_PKG_BUILD_SUC_MORE, prev_time, round_up_str((target_times.back() / prev_time_f - 1) * 100));
            else
                print_msg(MSG_PKG_BUILD_SUC_LESS, prev_time, round_up_str((1 - target_times.back() / prev_time_f)  * 100));
        }


        // Installing
        if (!dont_install)
            if (!c_install({ target_name }, tmp_args, true, std::make_pair(std::to_string(i + 1), std::to_string(target_infos.size()))))
                return false;


        // After install script
        if (!target_infos[i].aft_install.empty())
        {
            std::ofstream file(target_cache + "after-install");
            file << target_infos[i].aft_install;
            file.close();


            if (build_params)
                exec_cmd("chmod +x " + target_cache + "after-install && (cd " + target_cache + "source && " + BUILD_ARGS + " ../after-install)");
            else
                exec_cmd("chmod +x " + target_cache + "after-install && (cd " + target_cache + "source && ../after-install)");
        }
    }


    // Writing build times
    if (verbose)
        print_msg(MSGV_TIMES_WRITE);

    for (uint i = 0; i < target_infos.size(); i++)
    {
        const std::string& target_time_file = install_path + INSTALL_PATH + target_infos[i].name + "/build-time";

        if (std::filesystem::exists(target_time_file))
            exec_cmd(SU_CMD + " rm " + target_time_file);

        exec_cmd(" echo \"" + round_up_str(target_times[i]) + "\" | " + SU_CMD + " tee -a " + target_time_file + " > /dev/null");
    }


    return true;
}

std::optional<std::pair<std::vector<pkg_info>, std::vector<std::string>>> get_target_depends(const std::string &target, std::unordered_set<std::string>& prev_depends)
{
    std::vector<pkg_info> depends_info;
    std::vector<std::string> depends_paths;

    if (prev_depends.contains(target))
        return std::make_pair(depends_info, depends_paths);

    prev_depends.insert(target);


    // Getting target path and depends
    const std::string target_path = get_target_path(target);
    if (target_path.empty())
        return std::nullopt;

    const std::optional<pkg_info> info = get_pkg_info(target_path);
    if (!info.has_value())
        return std::nullopt;

    for (const std::string& depend : info.value().depends)
    {
        if (prev_depends.contains(depend))
            continue;

        const std::string depend_path = get_target_path(depend);
        if (depend_path.empty())
            return std::nullopt;

        const std::optional<pkg_info> depend_info = get_pkg_info(depend_path);
        if (!depend_info.has_value())
            return std::nullopt;

        const std::optional<pkg_info> depend_inst_info = get_pkg_info(INSTALL_PATH + depend + "/config.crs", false);
        if (depend_inst_info.has_value() && cross_stov(depend_inst_info->version) >= cross_stov(depend_info->version))
            continue;


        // Recursion
        const std::optional<std::pair<std::vector<pkg_info>, std::vector<std::string>>> depends_depends = get_target_depends(depend, prev_depends);
        if (!depends_depends.has_value())
            return std::nullopt;

        depends_info.insert(depends_info.end(), depends_depends.value().first.begin(), depends_depends.value().first.end());
        depends_paths.insert(depends_paths.end(), depends_depends.value().second.begin(), depends_depends.value().second.end());
    }

    depends_info.push_back(info.value());
    depends_paths.push_back(target_path);


    // Rebuild dependencies
    for (const std::string& rebuild_depend : info.value().rebuild_depends)
    {
        if (prev_depends.contains(rebuild_depend))
            continue;

        
        // Skip if it is not installed
        const std::optional<pkg_info> rebuild_inst_info = get_pkg_info(INSTALL_PATH + rebuild_depend + "/config.crs", false);
        if (!rebuild_inst_info.has_value())
            continue;


        // Getting package info
        const std::string rebuild_path = get_target_path(rebuild_depend);
        if (rebuild_path.empty())
            return std::nullopt;

        const std::optional<pkg_info> rebuild_info = get_pkg_info(rebuild_path);
        if (!rebuild_info.has_value())
            return std::nullopt;


        // Inserting package to queue
        const std::optional<std::pair<std::vector<pkg_info>, std::vector<std::string>>> rebuild_depends_depends = get_target_depends(rebuild_depend, prev_depends);
        if (!rebuild_depends_depends.has_value())
            return std::nullopt;

        depends_info.insert(depends_info.end(), rebuild_depends_depends.value().first.begin(), rebuild_depends_depends.value().first.end());
        depends_paths.insert(depends_paths.end(), rebuild_depends_depends.value().second.begin(), rebuild_depends_depends.value().second.end());
    }


    return std::make_pair(depends_info, depends_paths);
}
