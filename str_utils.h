#pragma once

#include <string>
#include <vector>
#include <string_view>

// string algo
std::vector<std::string> split(const std::string& str, char d);
std::string get_first_word(const std::string& str, char d);

/// cut first token from data and return it
std::string_view read_token(std::string_view& data, std::string_view delimiter);

/// convert from string to int
int convertToInt(std::string_view str);
int convertToIntDef(std::string_view str, int def_value) noexcept;
