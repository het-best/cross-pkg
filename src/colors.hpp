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

#pragma once

#include <string>



const std::string WHITE_COL = "\033[0m\033[0m";
const std::string RED_COL = "\033[0m\033[31m";
const std::string GREEN_COL = "\033[0m\033[32m";
const std::string BLUE_COL = "\033[0m\033[34m";
const std::string ORANGE_COL = "\033[1m\033[33m";
const std::string MAG_COL = "\033[0m\033[35m";
const std::string CYAN_COL = "\033[0m\033[36m";

const std::string PREFIX = ORANGE_COL + " " + WHITE_COL;
const std::string PKG_PREFIX = ORANGE_COL + " >>> " + BLUE_COL + "{ARG1} " + WHITE_COL;
const std::string ERR_PREFIX = RED_COL + " ERR " + WHITE_COL;
const std::string FLAG_PREFIX = CYAN_COL + " <-> " + WHITE_COL;