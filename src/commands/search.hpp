/* search - Prints out information about package
 *
 * Flags:
 * -O Shows all owned files
 */

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>


enum source_type
{
    SRC_URL,
    SRC_GIT,
};

enum depend_type
{
    DEP_MAKE,
    DEP_RUN,
};


struct pkg_info
{
    std::string name;
    std::string desc;
    std::string version;
    std::vector<std::tuple<source_type, std::string, std::string>> sources;
    std::vector<std::string> depends;
    std::string bef_build;
    std::string build;
    std::string aft_install;
};



void c_search(const std::string &target, const std::vector<std::pair<char, std::string>> &flags);

std::string get_target_path(const std::string& target);
std::optional<pkg_info> get_pkg_info(const std::filesystem::path &target_path, bool print_log = true);