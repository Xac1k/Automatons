#include "From.h"

AutomatonMoore From(MooreTable table) {
    AutomatonMoore automaton;
    automaton.startStateID = table.cells.at(0).newNodeID;

    automaton.states.reserve(table.headers.size());
    for (const auto& h : table.headers) {
        AutomatonMoore::State s;
        s.stateID = h.newNodeID;
        s.output = h.output;
        s.name = h.startNodeID;
        automaton.states.push_back(std::move(s));
    }

    const size_t cols = table.columns.size();
    automaton.transitions.reserve(table.headers.size() * cols);

    for (size_t i = 0; i < table.headers.size(); ++i) {
        for (size_t j = 0; j < cols; ++j) {
            const auto& header = table.headers[i];
            const auto& cell = table.cells.at(i * cols + j);

            AutomatonMoore::Transition t;
            t.startID = header.newNodeID;
            t.toID = cell.newNodeID;
            t.endID = cell.newNodeID;
            t.signal = table.columns[j];
            automaton.transitions.push_back(std::move(t));
        }
    }

    return automaton;
}

AutomatonMealy From(MealyTable table) {
    AutomatonMealy automaton;
    automaton.startStateID = table.headers.at(0);

    const size_t rows = table.headers.size();
    const size_t cols = table.columns.size();

    automaton.transitions.reserve(rows * cols);

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            const auto& cell = table.cells.at(i * cols + j);

            AutomatonMealy::Transitions t;
            t.startID = table.headers[i];
            t.signal = table.columns[j];
            t.endID = cell.endID;
            t.output = cell.output;
            automaton.transitions.push_back(std::move(t));
        }
    }

    return automaton;
}