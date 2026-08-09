#include "cmd.hpp"

#include <iostream>
#include <cstdio>
#include <array>



bool exec_cmd(const std::string &command)
{
    FILE* pipe = popen(command.c_str(), "r");
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