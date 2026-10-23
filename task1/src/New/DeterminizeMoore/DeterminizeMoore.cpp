#include "DeterminizeMoore.h"
#include "../../StringUtils/StringUtils.h"

#include <algorithm>
#include <deque>
#include <queue>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <bits/locale_facets_nonio.h>
#include <oneapi/tbb/task_group.h>

namespace {
using StateSet = std::vector<std::string>;

constexpr auto kEmptySymbol = "EMPTY";

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

    std::ranges::sort(states);
    states.erase(std::ranges::unique(states).begin(), states.end());
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
    return std::ranges::find(values, value) != values.end();
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

    std::ranges::sort(closure);
    return closure;
}

// Expand a set of states by replacing every state with its full epsilon-closure
// and then uniquifying the result.
StateSet CloseSet(
    const StateSet& states,
    const std::unordered_map<std::string, StateSet>& redefinition
) {
    StateSet result;
    for (const auto& s : states) {
        if (const auto it = redefinition.find(s); it != redefinition.end()) {
            result.insert(result.end(), it->second.begin(), it->second.end());
        } else {
            result.push_back(s);
        }
    }
    std::ranges::sort(result);
    result.erase(std::ranges::unique(result).begin(), result.end());
    return result;
}

void ValidateTransitions(MooreTable input) {
    for (const auto inputStateIDs = input.GetColumnIDs(); const auto& stateID : inputStateIDs) {
        auto& column = input.GetColumnByID(stateID).column;
        for (const auto& symbol : input.GetColumnByID(stateID).column.GetRowIDs()) {
            const auto targets = ParseStateList(column.GetRowByID(symbol).toColumnID);
            for (const auto& target : targets) {
                if (std::ranges::find(inputStateIDs, target) == inputStateIDs.end()) {
                    throw std::invalid_argument(
                        "Transition from '" + stateID + "' references unknown state '" += target + "'"
                    );
                }
            }
        }
    }
}

void ValidateInputData(const MooreTable& input, const std::string& startStateID) {
    const auto inputStateIDs = input.GetColumnIDs();
    if (inputStateIDs.empty()) {
        throw std::invalid_argument("Cannot determinize an empty MooreTable");
    }
    if (std::ranges::find(inputStateIDs, startStateID) == inputStateIDs.end()) {
        throw std::invalid_argument("Start state does not exist: " + startStateID);
    }

    ValidateTransitions(input);
}

std::unordered_map<std::string, StateSet> CacheEmptySuccessors(MooreTable& input) {
    const auto inputStateIDs = input.GetColumnIDs();
    std::unordered_map<std::string, std::vector<std::string>> emptySuccessors;

    for (const auto& stateID : inputStateIDs) {
        auto rowIDs = input.GetColumnByID(stateID).column.GetRowIDs();

        for (const auto& symbol : rowIDs) {
            if (symbol == kEmptySymbol) {
                const auto& targets = input.GetColumnByID(stateID).column.GetRowByID(symbol).toColumnID;
                auto targetsSet = ParseStateList(targets);
                emptySuccessors[stateID] = std::move(targetsSet);
            }
        }
    }

    return std::move(emptySuccessors);
}

std::unordered_map<std::string, StateSet>  BuildRedefinitionMap(MooreTable& input) {
    const auto inputStateIDs = input.GetColumnIDs();
    const auto emptySuccessors = CacheEmptySuccessors(input);

    std::unordered_map<std::string, StateSet> redefinition;
    for (const auto& stateID : inputStateIDs) {
        redefinition[stateID] = EpsilonClosure(stateID, emptySuccessors);
    }

    return std::move(redefinition);
}

std::set<std::string> BuildAlphabetWithoutEmpty(MooreTable& input) {
    const auto inputStateIDs = input.GetColumnIDs();
    std::set<std::string> alphabet;

    for (const auto& stateID : inputStateIDs) {
        const auto rowIDs = input.GetColumnByID(stateID).column.GetRowIDs();
        for (const auto& symbol : rowIDs) {
            if (symbol != kEmptySymbol) {
                alphabet.insert(symbol);
            }
        }
    }

    return std::move(alphabet);
}

void AddInitialStateToQueue(
    const std::string& startStateID,
    std::deque<StateSet>& queue,
    std::unordered_set<std::string>& created,
    const std::unordered_map<std::string, StateSet>& redefinitions
) {
    const StateSet& initial = redefinitions.at(startStateID);
    const std::string initialName = MakeStateName(initial);
    queue.push_back(initial);
    created.insert(initialName);
}

void DefineAnOutputSignal(MooreTable& input, MooreTable::TableColumnData& columnData, const StateSet& current) {
    columnData.payload.outputSignal.clear();
    for (const auto& stateID : current) {
        if (input.GetColumnByID(stateID).payload.outputSignal == "OUTPUT") {
            columnData.payload.outputSignal = "OUTPUT";
            break;
        }
    }
}

StateSet CollectUniqueTargetsSetFromState(MooreTable& input, const StateSet& current, const std::string& symbol) {
    StateSet targets;

    for (const auto& stateID : current) {
        const auto& rowIDs = input.GetColumnByID(stateID).column.GetRowIDs();
        if (!Contains(rowIDs, symbol)) continue;
        const auto& targetText = input.GetColumnByID(stateID).column.GetRowByID(symbol).toColumnID;
        auto parsed = ParseStateList(targetText);
        targets.insert(targets.end(), parsed.begin(), parsed.end());
    }
    std::ranges::sort(targets);
    targets.erase(std::ranges::unique(targets).begin(), targets.end());

    return std::move(targets);
}
}

MooreTable DeterminizeMoore(MooreTable input, const std::string& startStateID) {
    ValidateInputData(input, startStateID);
    std::set<std::string> alphabetWithoutEmpty = BuildAlphabetWithoutEmpty(input);
    auto redefinitions = BuildRedefinitionMap(input);

    MooreTable output;
    std::deque<StateSet> queue;
    std::unordered_set<std::string> created;
    AddInitialStateToQueue(startStateID, queue, created, redefinitions);

    auto addState = [&](const StateSet& states) {
        StateSet closed = CloseSet(states, redefinitions);
        if (const std::string name = MakeStateName(closed); created.insert(name).second) {
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
        DefineAnOutputSignal(input, columnData, current);

        output.AddNewColumn(currentName, columnData);

        for (const auto& symbol : alphabetWithoutEmpty) {
            auto targets = CollectUniqueTargetsSetFromState(input, current, symbol);

            MooreCellPayload cell;
            if (!targets.empty()) {
                StateSet closedTargets = CloseSet(targets, redefinitions);
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
