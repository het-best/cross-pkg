/* install - Installs package
 *
 * Flags:
 * -I ... Do not install packages or if argument is given install them to this path
 * -V     Enables verbose output
 */

#pragma once

#include <string>
#include <vector>



bool c_install(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags, bool hide_flags_msg = false,
    std::pair<std::string, std::string> number = {});
