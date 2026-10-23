#pragma once

#include "../AutomatTables/AutomatTables.h"
#include <string>

// Determinizes a MooreTable using the subset construction with support for
// unconditional (epsilon) transitions labelled EMPTY.
//
// Input cell.toColumnID may contain one state ID or a comma-separated list,
// e.g. "B,C". Empty strings mean that no transition exists.
// A state is accepting iff its MooreHeaderPayload::outputSignal == "OUTPUT".
//
// Unconditional transitions (symbol "EMPTY") are eliminated: the algorithm
// first computes the epsilon-closure of every original state and then applies
// the ordinary subset construction on the closed sets. The symbol EMPTY never
// appears in the resulting automaton.
//
// Throws std::invalid_argument for an unknown start state or target state.
MooreTable DeterminizeMoore(MooreTable input,
                            const std::string& startStateID);
