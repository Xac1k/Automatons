#pragma once
#include <string>
#include <vector>

void LTrim(std::string& str);
void RTrim(std::string& str);
std::string Trim(const std::string& str);
void ToLower(std::string& str);
std::vector<std::string> Separate(const std::string& text, const char& delim);