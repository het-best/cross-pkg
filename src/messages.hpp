#pragma once

#include <array>

#include "colors.hpp"
#include "defines.hpp"



enum msg_type
{
    // 000 - 099 General
    MSG_NO_CMD = 0,
    MSG_UNK_CMD = 1,
    MSG_UNK_FLAG = 2,
    MSG_NO_FLAG_ARG = 3,
    MSG_FLAG_ARG_INT = 4,
    MSG_NO_TARGET = 5,
    MSG_CONTINUE = 6,

    // 100 - 199 Build
    MSG_BUILD_ABORTING = 100,
    MSG_BUILDING_EXP = 101,
    MSG_BUILDING_IMP = 102,
    MSG_PKG_VALIDATION = 103,
    MSG_PKG_NEW = 104,
    MSG_PKG_LOWER_VER = 105,
    MSG_PKG_REBUILD = 106,
    MSG_PKG_HIGHER_VER = 107,
    MSG_PKG_HIGHER_VER_QW = 108,
    MSG_BUILD_START = 109,
    MSG_PKG_BUILDING = 110,
    MSG_PKG_BUILD_SUC = 111,
    MSG_PKG_BUILD_SUC_LESS = 112,
    MSG_PKG_BUILD_SUC_MORE = 113,
    MSG_PKG_BUILD_FAIL = 114,
    MSG_BUILD_SUC = 115,

    MSGV_GET_IMP_TARGETS = 116,
    MSGV_BUILD_ARGS = 117,
    MSGV_BUILD_STRIP = 118,
    MSGV_TIMES_WRITE = 119,

    // 200 - 299 Clear
    MSG_CACHE_CLEAR_NO_PKGS = 200,
    MSG_CACHE_CLEAR_PKGS = 201,
    MSG_CACHE_CLEARED = 202,

    // 300 - 399 Download
    MSG_DOWN_ABORTING = 300,
    MSG_PKG_READ_SRC = 301,
    MSG_PKG_ALR_DOWN = 302,
    MSG_PKG_DOWN_SRC = 303,
    MSG_PKG_GIT_COMMIT = 304,
    MSG_PKG_DOWN_FAIL = 305,
    MSG_PKG_EXTR_SRC = 306,

    MSGV_PKG_CHECK_CACHE = 307,
    MSGV_PKG_START_DOWN = 308,

    // 400 - 499 Install
    MSG_PKG_NO_CACHE = 400,
    MSG_PKG_INSTALLING = 401,
    MSG_PKG_NO_TO_INST = 402,
    MSG_PKG_MANIFEST = 403,
    MSG_PKG_INSTALLED = 404,

    MSGV_PKG_INST_FILE_FOUND = 405,
    MSGV_PKG_REM_FILES = 406,
    MSGV_PKG_INST_FILES = 407,

    // 500 - 599 List
    MSG_NO_INSTALLED = 500,
    MSG_TOTAL_INSTALLED = 501,

    // 600 - 699 Remove
    MSG_PKG_NOT_INSTALL = 600,
    MSG_PKG_CHECK_DEL = 601,
    MSG_PKG_HAS_DEPENDS = 602,
    MSG_PKG_REMOVING = 603,
    MSG_PKG_REMOVED = 604,
    MSG_PKG_NOT_REMOVED = 605,

    // 700 - 799 Search
    MSG_PKG_NOT_FOUND = 700,
    MSG_PKG_CONF_NO_EXT = 701,
    MSG_PKG_CONF_NOT_FOUND = 702,
    MSG_PKG_NO_FIELD = 703,
    MSG_PKG_NO_PREFIX = 704,
    MSG_PKG_UNK_PREFIX = 705,

    // 800 - 899 Update
    MSG_CROSS_VER = 800,
    MSG_CROSS_UPD = 801,
    MSG_PKGS_FETCH = 802,
    MSG_NO_PKGS_UPD = 803,
    MSG_PKGS_UPD = 804,
    MSG_PKGS_CAN_UPD = 805,
    MSG_PKGS_MAN_UPD = 806,
    MSG_UPDATE_SUCC = 807,
    MSG_UPDATE_FAIL = 808,

    MSGV_PKG_FETCH = 809,
    MSGV_CROSS_LICENSE = 810,

    // 900 - 999 Help
    MSG_CROSS_CMDS = 900,
    MSG_BUILD_INFO = 901,
    MSG_CLEAR_INFO = 902,
    MSG_DOWN_INFO = 903,
    MSG_INSTALL_INFO = 904,
    MSG_LIST_INFO = 905,
    MSG_REMOVE_INFO = 906,
    MSG_SEARCH_INFO = 907,
    MSG_UPDATE_INFO = 908,
    MSG_VERSION_INFO = 909,
};


const std::array gen_msgs =
{
    ERR_PREFIX + "No arguments were provided, see " + BLUE_COL + "help" + WHITE_COL + "\n",
    ERR_PREFIX + "Unknown argument \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\"\n",
    ERR_PREFIX + "Unknown flag \"" + MAG_COL + "{ARG1}" + WHITE_COL + "\"\n",
    ERR_PREFIX + "No flag arguments were given to \"" + MAG_COL + "{ARG1}" + WHITE_COL + "\"\n",
    ERR_PREFIX + "Flag arguments must be integer \"" + MAG_COL + "{ARG1}" + WHITE_COL + "\"\n",
    ERR_PREFIX + "No targets were provided\n",
    PREFIX + CYAN_COL + "Continue?" + WHITE_COL + ": Press Enter to continue or Ctrl+C to abort\n",
};

const std::array build_msgs =
{
    PREFIX + "Aborting building\n",
    PREFIX + "Building explicit: " + BLUE_COL + "{ARG1}" + WHITE_COL + "\n",
    PREFIX + "Building implicit: " + BLUE_COL + "{ARG1}" + WHITE_COL + "\n",
    PREFIX + "Validating packages...\n",
    PREFIX + "Building new package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\"" + GREEN_COL + " v{ARG2}" + WHITE_COL + "\n",
    PREFIX + "Updating already installed package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\": "
        + ORANGE_COL + "{ARG2}" + WHITE_COL + " -> " + GREEN_COL + "{ARG3}" + WHITE_COL + "\n",
    PREFIX + "Rebuilding package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\"\n",
    PREFIX + "Downgrading already installed package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\": "
        + GREEN_COL + "{ARG2}" + WHITE_COL + " -> " + ORANGE_COL + "{ARG3}" + WHITE_COL + "\n",
    PREFIX + "Package version is lower than already installed one, still install? " + CYAN_COL + "[Y/yy/n]" + WHITE_COL + ": ",
    PREFIX + "Starting building...\n",
    PKG_PREFIX + "Building package ({ARG2})\n",
    PKG_PREFIX + "Successfully built package ({ARG2}) in " + BLUE_COL + "{ARG3}" + WHITE_COL + "s\n",
    PREFIX + "Previous build took " + BLUE_COL + "{ARG1}" + WHITE_COL + "s, " + GREEN_COL + "{ARG2}" + WHITE_COL + "% faster this time\n",
    PREFIX + "Previous build took " + BLUE_COL + "{ARG1}" + WHITE_COL + "s, " + ORANGE_COL + "{ARG2}" + WHITE_COL + "% longer this time\n",
    PKG_PREFIX + "Package building failed :[\n",
    PKG_PREFIX + "Package building was successful :D\n",

    PREFIX + "Getting implicit targets\n",
    PKG_PREFIX + "Build arguments: " + CYAN_COL + "{ARG2}" + WHITE_COL + "\n",
    PKG_PREFIX + "Striping compiled binaries\n",
    PREFIX + "Writing build times\n",
};

const std::array clear_msgs =
{
    PREFIX + "No packages will be cleaned\n",
    PREFIX + "Next packages cache will be cleaned: " + BLUE_COL + "{ARG1}" + WHITE_COL + "\n",
    PREFIX + "Cache was successfully cleared\n",
};

const std::array down_msgs =
{
    PREFIX + "Aborting downloading\n",
    PKG_PREFIX + "Reading sources\n",
    PKG_PREFIX + "Source " + CYAN_COL + "{ARG2}" + WHITE_COL + " already downloaded, skipping...\n",
    PKG_PREFIX + "Downloading " + CYAN_COL + "{ARG2}" + WHITE_COL + "\n",
    PKG_PREFIX + "Git source: " + CYAN_COL + "{ARG2}" + WHITE_COL + " has more than 1 @, will be using the last one\n",
    ERR_PREFIX + "Cannot download source \"" + CYAN_COL + "{ARG1}" + WHITE_COL + "\"\n",
    PREFIX + "Extracting sources\n",

    PKG_PREFIX + "Checking package cache\n",
    PKG_PREFIX + "Starting downloading sources\n",
};

const std::array install_msgs =
{
    ERR_PREFIX + "Package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\" cache is not found/valid\n",
    PKG_PREFIX + "Installing package ({ARG2})\n",
    PKG_PREFIX + "Nothing to install\n",
    PKG_PREFIX + "Creating manifest\n",
    PKG_PREFIX + "Successfully installed package ({ARG2})\n",

    PREFIX + "Found \"" + CYAN_COL + "{ARG1}" + WHITE_COL + "\"\n",
    PKG_PREFIX + "Removing previous package files...\n",
    PKG_PREFIX + "Copying package files...\n",
};

const std::array list_msgs =
{
    PREFIX + "No installed packages found! (" + MAG_COL + "How did you managed to do this" + WHITE_COL + "?)\n",
    PREFIX + "Total packages: " + GREEN_COL + "{ARG1}" + WHITE_COL + "\n",
};

const std::array remove_msgs =
{
    ERR_PREFIX + "Package \"{ARG1}\" not installed\n",
    PREFIX + GREEN_COL + "{ARG1}" + WHITE_COL + " Checking if package removable\n",
    ERR_PREFIX + "Package \"{ARG1}\" has dependents: " + MAG_COL + "{ARG2}" + WHITE_COL + "\n",
    PREFIX + GREEN_COL + "{ARG1}" + WHITE_COL + " Removing package\n",
    PREFIX + GREEN_COL + "{ARG1}" + WHITE_COL + " Removed successfully\n",
    ERR_PREFIX + "Package \"{ARG1}\" has not been removed, maybe try to delete it by hand\n",
};

const std::array search_msgs =
{
    ERR_PREFIX + "Package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\" wasn't found, maybe you need to update " + MAG_COL + "$CROSS_PATH" + WHITE_COL + "\n",
    ERR_PREFIX + "Package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\" configure file needs to have " + CYAN_COL  + ".crs" + WHITE_COL + " extension\n",
    ERR_PREFIX + "Package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\" configure file wasn't found\n",
    ERR_PREFIX + "Package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\" configure file doesnt have " + CYAN_COL + "-{ARG2}-" + WHITE_COL + " field\n",
    ERR_PREFIX + "Package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\" configure file source field doesnt have prefix\n",
    ERR_PREFIX + "Package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\" configure file source field has unknown prefix: \"" + CYAN_COL + "{ARG2}" + WHITE_COL + "\"\n",
};

const std::array update_msgs =
{
    PREFIX + "cross-pkg v" + CROSS_VERSION + "\n",
    PREFIX + "Cross can be updated! " + ORANGE_COL + CROSS_VERSION + WHITE_COL + " -> " + GREEN_COL + "{ARG1}" + WHITE_COL + "\n",
    PREFIX + "Fetching packages for updates...\n",
    PREFIX + GREEN_COL + "No packages require update!" + WHITE_COL + "\n",
    PREFIX + GREEN_COL + "{ARG1}" + WHITE_COL + " packages can be updated, " + ORANGE_COL + "{ARG2}" + WHITE_COL + " needed to be checked manually \n",
    "   Package \"" + BLUE_COL + "{ARG1}" + WHITE_COL + "\" can be updated: " + ORANGE_COL + "{ARG2}" + WHITE_COL + " -> " + GREEN_COL + "{ARG3}" + WHITE_COL + "\n",
    ORANGE_COL + "   Package \"" + BLUE_COL + "{ARG1}" + ORANGE_COL + "\" cannot be updated automatically, check it manually" + WHITE_COL + "\n",
    PREFIX + "Packages were updated successfully\n",
    PREFIX + "Update failed\n",

    PKG_PREFIX + "Fetching package...\n",
    ORANGE_COL + "\ncross-pkg  Copyright (C) 2026 Het Best\n" +
    "This program comes with ABSOLUTELY NO WARRANTY.\n" +
    "This is free software, and you are welcome to redistribute it\n" +
    "under certain conditions." + WHITE_COL + "\n",
};

const std::array help_msgs =
{
    PREFIX + "cross-pkg v" + CROSS_VERSION + "\n\n",
    PREFIX + BLUE_COL + "build    " + WHITE_COL + "Builds and installed packages\n",
    PREFIX + BLUE_COL + "clear    " + WHITE_COL + "Clears cache\n",
    PREFIX + BLUE_COL + "download " + WHITE_COL + "Downloads package sources\n",
    PREFIX + BLUE_COL + "install  " + WHITE_COL + "Installs package\n",
    PREFIX + BLUE_COL + "list     " + WHITE_COL + "Prints out information about installed packages and their versions\n",
    PREFIX + BLUE_COL + "remove   " + WHITE_COL + "Removes package\n",
    PREFIX + BLUE_COL + "search   " + WHITE_COL + "Prints out information about package\n",
    PREFIX + BLUE_COL + "update   " + WHITE_COL + "Updates all packages and fetches their latest versions from the repos\n",
    PREFIX + BLUE_COL + "version  " + WHITE_COL + "Print out information about cross version and fetches for updates\n",
};



void print_msg(msg_type msg, const std::string &arg1 = "", const std::string &arg2 = "", const std::string &arg3 = "");
std::string replace_occ(std::string str, const std::string &occurance, const std::string &with);
