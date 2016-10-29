#pragma once

#include <vector>
#include <string>

std::vector<std::string> glob(const std::string& str);
std::string stripBase(const std::string& str);
std::string stripExtension(const std::string& str);
