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
