/* clear - Clears cache
 *
 * Flags:
 * -Y     Auto-yes, autoresponds yes to all messages (including continue prompts)
 * -E ... Exclude packages from clearing
 * -I ... Include packages to clearing
 * -V     Enables verbose output
 */

#pragma once

#include <string>
#include <vector>



void c_clear(const std::vector<std::pair<char, std::string>> &flags);
