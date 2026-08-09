/* build - Builds and installed packages
 *
 * Flags:
 * -B     Remove standard build params
 * -P     Show targets paths instead of names
 * -Y     Auto-yes, autoresponds yes to all messages (including continue prompts)
 * -D     Disabled checking dependencies
 * -I ... Do not install packages or if argument is given install them to this path
 * -R     Force redownloading sources even if they are already present
 * -P     Do not remove already unpacked/compiled source
 * -S     Do not download sources
 * -V     Enables verbose output
 */

#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "search.hpp"


bool c_build(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags);
std::optional<std::pair<std::vector<pkg_info>, std::vector<std::string>>> get_target_depends(const std::string &target, std::unordered_set<std::string>& prev_depends);