/* version - Print out information about cross version and fetches for updates
 *
 * Flags:
 * -F Do not fetch cross-pkg version
 */

#pragma once

#include <string>
#include <vector>



void c_version(const std::vector<std::pair<char, std::string>> &flags);