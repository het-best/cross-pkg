#include <string>
#include <vector>
#include <cstdint>

#include "split.hpp"

#include <cmath>
#include <format>


// CREDIT: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
std::vector<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::string str_ = str;

    std::vector<std::string> tokens;
    size_t pos = 0;

    while ((pos = str_.find(delimiter)) != std::string::npos)
    {
        std::string token = str_.substr(0, pos);
        tokens.push_back(token);
        str_.erase(0, pos + delimiter.length());
    }
    tokens.push_back(str_);


    return tokens;
}


uint cross_stoi(const std::string& str)
{
    std::string final_str;

    for (const char c : str)
    {
        if (c >= '0' && c <= '9')
            final_str += c;
    }

    if (final_str.empty())
        return 0;

    return std::stoi(final_str);
}

float cross_stof(const std::string& str)
{
    std::string final_str;

    for (const char c : str)
    {
        if (c >= '0' && c <= '9')
            final_str += c;
        else if (c == '.')
            final_str += '.';
    }

    if (final_str.empty())
        return 0;

    return std::stof(final_str);
}


uint64_t cross_stov(const std::string& str)
{
    constexpr uint VAR_SIZE = 8;

    std::string final_str = "1";

    for (const char c : str)
    {
        if (c >= '0' && c <= '9')
            final_str += c;
    }

    while (final_str.size() < VAR_SIZE)
    {
        final_str += "0";
    }

    return std::stoul(final_str);
}


std::string round_up_str(const float &value, const uint &decimals)
{
    return std::format("{:.{}f}", value, decimals);
}
