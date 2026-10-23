#include "DeterminizeMoore.h"

#include <algorithm>
#include <cctype>
#include <deque>
#include <queue>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {
using StateSet = std::vector<std::string>;

constexpr const char* kEmptySymbol = "EMPTY";

std::string Trim(std::string value) {
    const auto notSpace = [](unsigned char c) { return !std::isspace(c); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
    value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
    return value;
}

StateSet ParseStateList(const std::string& text) {
    StateSet states;
    std::size_t begin = 0;
    while (begin <= text.size()) {
        const std::size_t comma = text.find(',', begin);
        const std::size_t end = (comma == std::string::npos) ? text.size() : comma;
        std::string state = Trim(text.substr(begin, end - begin));
        if (!state.empty()) states.push_back(std::move(state));
        if (comma == std::string::npos) break;
        begin = comma + 1;
    }

    std::sort(states.begin(), states.end());
    states.erase(std::unique(states.begin(), states.end()), states.end());
    return states;
}

std::string MakeStateName(const StateSet& states) {
    std::string result;
    for (const auto& state : states) {
        if (!result.empty()) result += ',';
        result += state;
    }
    return result;
}

bool Contains(const std::vector<std::string>& values, const std::string& value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

// Compute the epsilon-closure of a single state: the set of all states
// reachable from it by zero or more EMPTY transitions, including itself.
// Cycles are handled by the visited set; they are not considered an error.
StateSet EpsilonClosure(
    const std::string& start,
    const std::unordered_map<std::string, std::vector<std::string>>& emptySuccessors)
{
    StateSet closure;
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        const std::string cur = std::move(q.front());
        q.pop();
        closure.push_back(cur);

        const auto it = emptySuccessors.find(cur);
        if (it == emptySuccessors.end()) continue;

        for (const auto& nxt : it->second) {
            if (visited.insert(nxt).second) {
                q.push(nxt);
            }
        }
    }

    std::sort(closure.begin(), closure.end());
    return closure;
}

// Expand a set of states by replacing every state with its full epsilon-closure
// and then uniquifying the result.
StateSet CloseSet(
    const StateSet& states,
    const std::unordered_map<std::string, StateSet>& redefinition)
{
    StateSet result;
    for (const auto& s : states) {
        const auto it = redefinition.find(s);
        if (it != redefinition.end()) {
            result.insert(result.end(), it->second.begin(), it->second.end());
        } else {
            result.push_back(s);
        }
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}
} // namespace

MooreTable DeterminizeMoore(MooreTable input, const std::string& startStateID) {
    const auto inputStateIDs = input.GetColumnIDs();
    if (inputStateIDs.empty()) {
        throw std::invalid_argument("Cannot determinize an empty MooreTable");
    }
    if (std::find(inputStateIDs.begin(), inputStateIDs.end(), startStateID) == inputStateIDs.end()) {
        throw std::invalid_argument("Start state does not exist: " + startStateID);
    }

    // Collect the input alphabet and the rows present in each column.
    // EMPTY is recorded separately and will never appear in the output.
    std::set<std::string> alphabet;
    std::unordered_map<std::string, std::vector<std::string>> rowsByState;
    std::unordered_map<std::string, std::vector<std::string>> emptySuccessors;

    for (const auto& stateID : inputStateIDs) {
        auto rowIDs = input.GetColumnByID(stateID).column.GetRowIDs();
        rowsByState.emplace(stateID, rowIDs);

        for (const auto& symbol : rowIDs) {
            if (symbol == kEmptySymbol) {
                const auto& targetText =
                    input.GetColumnByID(stateID).column.GetRowByID(symbol).toColumnID;
                auto parsed = ParseStateList(targetText);
                emptySuccessors[stateID] = std::move(parsed);
            } else {
                alphabet.insert(symbol);
            }
        }
    }

    // Validate all transition targets (including EMPTY) up front.
    for (const auto& stateID : inputStateIDs) {
        auto& column = input.GetColumnByID(stateID).column;
        for (const auto& symbol : rowsByState.at(stateID)) {
            const auto targets = ParseStateList(column.GetRowByID(symbol).toColumnID);
            for (const auto& target : targets) {
                if (std::find(inputStateIDs.begin(), inputStateIDs.end(), target) ==
                    inputStateIDs.end()) {
                    throw std::invalid_argument(
                        "Transition from '" + stateID + "' references unknown state '" +
                        target + "'");
                }
            }
        }
    }

    // ------------------------------------------------------------------
    // Step 1. Build the redefinition map:
    //   original state  ->  its epsilon-closure (including itself)
    // ------------------------------------------------------------------
    std::unordered_map<std::string, StateSet> redefinition;
    for (const auto& stateID : inputStateIDs) {
        redefinition[stateID] = EpsilonClosure(stateID, emptySuccessors);
    }

    // ------------------------------------------------------------------
    // Step 2. Ordinary subset construction, but every newly formed subset
    // is immediately closed under the redefinition map.
    // ------------------------------------------------------------------
    MooreTable output;
    std::deque<StateSet> queue;
    std::unordered_set<std::string> created;

    // Initial state = epsilon-closure of the given start state.
    StateSet initial = redefinition.at(startStateID);
    const std::string initialName = MakeStateName(initial);
    queue.push_back(initial);
    created.insert(initialName);

    auto addState = [&](const StateSet& states) {
        // Translate (close) the candidate set before deciding whether it is new.
        StateSet closed = CloseSet(states, redefinition);
        const std::string name = MakeStateName(closed);
        if (created.insert(name).second) {
            queue.push_back(std::move(closed));
        }
    };

    while (!queue.empty()) {
        StateSet current = std::move(queue.front());
        queue.pop_front();
        const std::string currentName = MakeStateName(current);

        MooreTable::TableColumnData columnData;
        columnData.payload.equivalenceClass = 0;
        columnData.payload.stateID = currentName;
        columnData.payload.outputSignal.clear();
        // A composite state is accepting if any of its members is accepting.
        for (const auto& stateID : current) {
            if (input.GetColumnByID(stateID).payload.outputSignal == "OUTPUT") {
                columnData.payload.outputSignal = "OUTPUT";
                break;
            }
        }

        // Insert the column before processing its transitions.
        output.AddNewColumn(currentName, columnData);

        // Only ordinary (non-EMPTY) symbols appear in the resulting automaton.
        for (const auto& symbol : alphabet) {
            StateSet targets;
            for (const auto& stateID : current) {
                const auto& rowIDs = rowsByState.at(stateID);
                if (!Contains(rowIDs, symbol)) continue;
                const auto& targetText =
                    input.GetColumnByID(stateID).column.GetRowByID(symbol).toColumnID;
                auto parsed = ParseStateList(targetText);
                targets.insert(targets.end(), parsed.begin(), parsed.end());
            }
            std::sort(targets.begin(), targets.end());
            targets.erase(std::unique(targets.begin(), targets.end()), targets.end());

            MooreCellPayload cell;
            if (!targets.empty()) {
                // Close the target set under epsilon transitions before naming it.
                StateSet closedTargets = CloseSet(targets, redefinition);
                cell.toColumnID = MakeStateName(closedTargets);
                addState(closedTargets);
            } else {
                cell.toColumnID.clear();
            }
            output.GetColumnByID(currentName).column.AddNewRow(symbol, cell);
        }
    }

    return output;
}
