#pragma once
#include <string>
#include "../Automatons.h"

AutomatonMealy MealyParse(const std::string& text);
AutomatonMoore MooreParse(const std::string& text);