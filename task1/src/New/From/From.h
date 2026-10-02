#pragma once
#include "../AutomatTables/AutomatTables.h"
#include "../../Old/Automatons.h"

MooreTable From(const AutomatonMoore& automaton);
MealyTable From(const AutomatonMealy& automaton);