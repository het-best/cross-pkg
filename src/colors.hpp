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