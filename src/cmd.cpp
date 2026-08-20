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

#include "cmd.hpp"

#include <iostream>
#include <cstdio>
#include <array>



bool exec_cmd(const std::string &command)
{
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("popen() failed");
    std::array<char, 256> buffer{};


    // Reading the stream
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        std::cout << buffer.data() << std::flush;
    }


    return pclose(pipe) == 0;
}

std::string exec_cmd_echo(const std::string &command)
{
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("popen() failed");
    std::array<char, 256> buffer{};


    // Reading the stream
    std::string result;

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    pclose(pipe);


    return result;
}