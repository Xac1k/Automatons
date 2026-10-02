#pragma once
#include <iostream>
#include "../Automatons.h"

void Print(const AutomatonMoore& automaton, std::ostream& os = std::cout);
void Print(const AutomatonMealy& automaton, std::ostream& os = std::cout);
void Print(const MealyTable& table, std::ostream& out = std::cout);
void Print(const MooreTable& table, std::ostream& out = std::cout);