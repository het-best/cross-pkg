/* download - Downloads package sources
 *
* Flags:
 * -R Force redownloading sources even if they are already present
 * -P Do not remove already unpacked/compiled source
 * -V Enables verbose output
 */

#pragma once

#include <string>
#include <vector>



bool c_download(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags, bool hide_flags_msg = false);
