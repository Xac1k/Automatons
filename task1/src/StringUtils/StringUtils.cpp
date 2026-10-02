#include <string>
#include <algorithm>
#include <ranges>
#include <sstream>
#include <vector>
#include "StringUtils.h"

void LTrim(std::string& str)
{
    const auto it = std::ranges::find_if_not(
        str,
        [] (const char& c){return std::isspace(c) || c == '\n';}
    );
    str.erase(str.begin(), it);
}

void RTrim(std::string& str)
{
    const auto rit = std::ranges::find_if_not(
        str.rbegin(),
        str.rend(),
        [](const char& c){return std::isspace(c) || c == '\n';}
    );
    str.erase(rit.base(), str.end());
}

void Trim(std::string& str)
{
    LTrim(str);
    RTrim(str);
}

void ToLower(std::string& str)
{
    std::ranges::transform(
        str,
        str.begin(),
        [](const char& c){ return std::tolower(c); }
    );
}

std::vector<std::string> Separate(const std::string& text, const char& delim)
{
    std::string copiedText = text;
    Trim(copiedText);
    std::istringstream stream(copiedText);
    std::vector<std::string> res;
    std::string elt;

    while(std::getline(stream, elt, delim)) {
        Trim(elt);
        if(!elt.empty())
            res.push_back(elt);
    }

    return res;
}