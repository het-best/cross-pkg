/* update - Updates all packages and fetches their latest versions from the repos
 *
 * Flags:
 * -Y     Auto-yes, autoresponds yes to all messages (including continue prompts)
 * -S ... Fetching sleep time in seconds
 * -F     Do not fetch version
 */

#pragma once

#include <string>
#include <vector>



void c_update(const std::vector<std::pair<char, std::string>> &flags);

std::pair<std::string, std::string> get_version(const std::string &url, std::string current_version, uint sleep_time = 1);