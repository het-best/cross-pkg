#pragma once

#include <string>
#include <vector>
#include <cstdint>



std::vector<std::string> split(const std::string& str, const std::string& delimiter);

uint cross_stoi(const std::string& str);
float cross_stof(const std::string& str);
uint64_t cross_stov(const std::string& str);

std::string round_up_str(const float& value, const uint& decimals = 3);
