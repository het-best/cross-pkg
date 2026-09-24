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
