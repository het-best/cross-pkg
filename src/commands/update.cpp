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
        if (!pkg_info.has_value() || (pkg_info.has_value() && pkg_info->sources.empty()))
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


    // Changing packages info
    std::vector<std::string> build_targets;
    build_targets.reserve(sources_to_upd.size() - manual_update);

    for (const auto& [current_info, url, version, name] : sources_to_upd)
    {
        if (url == "Unsupported provider")
            continue;


        std::string target_path = get_target_path(name);


        // Making config string
        std::string config_str;

        config_str += "-desc-\n\t" + current_info.desc + "\n";
        config_str += "-version-\n\t" + version + "\n";

        config_str += "-sources-\n";
        for (const std::tuple<source_type, std::string, std::string>& source : current_info.sources)
        {
            if (source == current_info.sources.front())
            {
                config_str += "\turl+" + url + "\n";
                continue;
            }

            std::string prefix;
            if (std::get<0>(source) == SRC_URL)
                prefix = "url";
            else if (std::get<0>(source) == SRC_GIT)
                prefix = "git";

            config_str += "\t" + prefix + "+" + std::get<1>(source) + " " + std::get<2>(source) + "\n";
        }

        if (!current_info.depends.empty())
        {
            config_str += "-depends-\n";
            for (const std::string& depend : current_info.depends)
            {
                config_str += "\t" + depend + "\n";
            }
        }

        if (!current_info.bef_build.empty())
        {
            config_str += "-before-build-\n";
            for (const std::string& build_str : split(current_info.bef_build, "\n"))
            {
                if (build_str != "#!/bin/sh -e")
                    config_str += "\t" + build_str + "\n";
            }
        }

        config_str += "-build-\n";
        for (const std::string& build_str : split(current_info.build, "\n"))
        {
            if (build_str != "#!/bin/sh -e")
                config_str += "\t" + build_str + "\n";
        }

        if (!current_info.aft_install.empty())
        {
            config_str += "-after-install-\n";
            for (const std::string& build_str : split(current_info.aft_install, "\n"))
            {
                if (build_str != "#!/bin/sh -e")
                    config_str += "\t" + build_str + "\n";
            }
        }


        // Writing data
        exec_cmd("rm " + target_path);
        std::ofstream file(config_str);
        file << config_str;


        build_targets.push_back(name);
    }


    // Building
    std::vector<std::pair<char, std::string>> tmp_flags;
    if (autoyes)
        tmp_flags.push_back({'y', ""});
    if (verbose)
        tmp_flags.push_back({'v', ""});

    if (c_build(build_targets, tmp_flags))
        print_msg(MSG_UPDATE_SUCC);
    else
        print_msg(MSG_UPDATE_FAIL);
}


std::pair<std::string, std::string> get_version(const std::string &url, std::string current_version, uint sleep_time)
{
    if (url.find("github.com") != std::string::npos)
    {
        // Getting only https://github.com/XXX/XXX
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

        if (exec_cmd("curl -sIL -o /dev/null -w \"%{http_code}\n\" " + possible_url + " | grep -q \"^200$\""))
            download_url = possible_url;
        else
            download_url = base_url + "archive/refs/tags/" + (has_v ? "v" : "") + version + ".tar.gz";


        // Checking version
        if (cross_stov(version) > cross_stov(current_version))
            return std::make_pair(download_url, version);
    }
    else if (url.find("https://ftp.gnu.org/gnu/") != std::string::npos)
    {
        std::vector<std::string> splitted_url = split(url, "/");
        std::string base_url;

        for (uint i = 0; i < 5; i++)
        {
            base_url += splitted_url[i] + "/";
        }


        // Getting package name
        std::string package_name;
        std::vector<std::string> pkg_name_poss = split(splitted_url[5], "-");

        for (uint i = 0; i < pkg_name_poss.size(); i++)
        {
            if (pkg_name_poss[i].find_first_of("0123456789") == 0)
            {
                for (uint v = 0; v < i; v++)
                {
                    package_name += pkg_name_poss[v] + "-";
                }
                package_name.erase(package_name.length() - 1);
            }
        }

        // Getting version
        std::string version = exec_cmd_echo("curl -s " + base_url + " | grep -oE '" + package_name + "-[0-9]+\\.[0-9]+(\\.[0-9]+)*' | "
                                                                                                     "sort -V | tail -n 1 | sed 's/" + package_name + "-//'");

        if (version.empty())
            return {};

        version.erase(version.length() - 1);
        if (cross_stov(version) < cross_stov(current_version))
            return std::make_pair(base_url + package_name + "-" + version + ".tar.gz", version);
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
