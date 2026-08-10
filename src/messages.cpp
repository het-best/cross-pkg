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

#include "messages.hpp"

#include <iostream>
#include <ostream>

#include "colors.hpp"



void print_msg(const msg_type msg, const std::string &arg1, const std::string &arg2, const std::string &arg3 )
{
    // Getting message string
    std::string msg_str;

    if (const uint type_i  = static_cast<uint>(msg); type_i < 100)
        msg_str = gen_msgs[type_i];
    else if (type_i < 200)
        msg_str = build_msgs[type_i - 100];
    else if (type_i < 300)
        msg_str = clear_msgs[type_i - 200];
    else if (type_i < 400)
        msg_str = down_msgs[type_i - 300];
    else if (type_i < 500)
        msg_str = install_msgs[type_i - 400];
    else if (type_i < 600)
        msg_str = list_msgs[type_i - 500];
    else if (type_i < 700)
        msg_str = remove_msgs[type_i - 600];
    else if (type_i < 800)
        msg_str = search_msgs[type_i - 700];
    else if (type_i < 900)
        msg_str = update_msgs[type_i - 800];
    else if (type_i < 1000)
        msg_str = help_msgs[type_i - 900];

    // Arguments
    msg_str = replace_occ(msg_str, "{ARG1}", arg1);
    msg_str = replace_occ(msg_str, "{ARG2}", arg2);
    msg_str = replace_occ(msg_str, "{ARG3}", arg3);


    std::cout << msg_str << WHITE_COL;
}

std::string replace_occ(std::string str, const std::string &occurance, const std::string &with)
{
    size_t start_pos = 0;

    while ((start_pos = str.find(occurance, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, occurance.length(), with);
        start_pos += with.length();
    }


    return str;
}
