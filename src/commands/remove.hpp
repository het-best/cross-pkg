/* remove - Removes package
 *
 * Flags:
 * -V Enables verbose output
 */

#pragma once

#include <string>
#include <vector>



void c_remove(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags);
