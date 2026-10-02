#pragma once
#include <tuple>
#include <vector>
#include "Automatons.h"
#include <regex>
#include <set>
#include <string>
#include <iomanip>



std::tuple<std::vector<std::string>, std::vector<std::string>> GetHeaderOfMealyTable(const AutomatonMealy& mealy)
{
    std::set<std::string> stateIDs;
    std::set<std::string> signalsColumn;

    for (const auto& transition : mealy.transitions) {
        stateIDs.insert(transition.startID);
        signalsColumn.insert(transition.signal);
    }

    std::vector sortedStateIDs(stateIDs.begin(), stateIDs.end());
    std::vector sortedSignalsColumn(signalsColumn.begin(), signalsColumn.end());
    std::sort(sortedStateIDs.begin(), sortedStateIDs.end());
    std::sort(sortedSignalsColumn.begin(), sortedSignalsColumn.end());

    return {sortedStateIDs, sortedSignalsColumn};
}

MealyTable GetMealyTable(const AutomatonMealy& mealy) {
    auto [stateIDs, signalsColumn] = GetHeaderOfMealyTable(mealy);

    auto FindX = [stateIDs] (const std::string& stateID) {
        for (size_t iter = 0; iter < stateIDs.size(); iter++) {
            if (stateID ==  stateIDs[iter]) {
                return iter;
            }
        }
    };

    auto FindY = [signalsColumn] (const std::string& signal) {
        for (size_t iter = 0; iter < signalsColumn.size(); iter++) {
            if (signal ==  signalsColumn[iter]) {
                return iter;
            }
        }
    };

    std::vector<MealyCell> cells(stateIDs.size() * signalsColumn.size());

    for (const auto& transition : mealy.transitions) {
        MealyCell cell;

        cell.endID = transition.endID;
        cell.output = transition.output;
        auto position = FindX(transition.startID) + FindY(transition.signal) * stateIDs.size();

        cells.at(position) = cell;
    }

    return {stateIDs, signalsColumn, cells};
}

std::vector<MooreToMealyHeader> GetMooreHeaders(MealyTable& mealyTable) {
    std::set<MooreToMealyHeader> mooreHeaders;
    for (auto& [endID, output] : mealyTable.cells) {
        mooreHeaders.insert({endID, "", output});
    }
    std::vector sortedMooreHeaders(mooreHeaders.begin(), mooreHeaders.end());
    std::sort(sortedMooreHeaders.begin(), sortedMooreHeaders.end());

    size_t iter = 0;
    for (auto& headerCell : sortedMooreHeaders) {
        headerCell.newNodeID = "Z" + std::to_string(iter++);
    }

    return sortedMooreHeaders;
}

MooreTable Mealy2Moore(const AutomatonMealy& mealy) {
    auto mealyTable = GetMealyTable(mealy);

    MooreTable moreTable;
    moreTable.headers = GetMooreHeaders(mealyTable);
    moreTable.columns = mealyTable.columns;
    moreTable.cells.resize(moreTable.headers.size() * moreTable.columns.size());
    auto cellsPerLine = moreTable.headers.size();

    for (size_t y = 0; y < moreTable.columns.size(); y++) {
        for (size_t x = 0; x < moreTable.headers.size(); x++) {
            auto& targetCell = moreTable.cells.at(cellsPerLine * y + x);
            auto header = moreTable.headers.at(x);
            auto signal = moreTable.columns.at(y);

            auto mealyHeaderIt = std::ranges::find(mealyTable.headers, header.startNodeID);
            auto mealyX = mealyHeaderIt - mealyTable.headers.begin();
            auto mealyCell = mealyTable.cells.at(mealyX + y * mealyTable.columns.size());

            auto targetHeader = std::ranges::find_if(moreTable.headers, [mealyCell](auto& header) {
                return header.output == mealyCell.output && header.startNodeID == mealyCell.endID;
            });

            targetCell.newNodeID = targetHeader->newNodeID;
        }
    }

    return moreTable;
}

std::vector<MooreToMealyHeader> GetMooreHeaders(const AutomatonMoore& moore) {
    std::vector<MooreToMealyHeader> mooreHeaders;
    for (auto& [stateID, output, name] : moore.states) {
        mooreHeaders.push_back({stateID, name, output});
    }
    std::sort(mooreHeaders.begin(), mooreHeaders.end(),
    [](const MooreToMealyHeader& a, const MooreToMealyHeader& b) {
        return a.newNodeID < b.newNodeID;
    });

    return mooreHeaders;
}

std::vector<std::string> GetMooreColumns(const AutomatonMoore& moore) {
    std::set<std::string> columns;
    for (const auto& transition : moore.transitions) {
        columns.insert(transition.signal);
    }
    std::vector sortedColumns(columns.begin(), columns.end());
    std::sort(sortedColumns.begin(), sortedColumns.end());

    return sortedColumns;
}

MooreTable GetMooreTable(const AutomatonMoore& moore) {
    auto headers = GetMooreHeaders(moore);
    auto columns = GetMooreColumns(moore);
    std::vector<MooreCell> cells(headers.size() * columns.size());

    auto FindX = [headers] (const std::string& stateID) {
        for (size_t iter = 0; iter < headers.size(); iter++) {
            if (stateID == headers[iter].newNodeID) {
                return iter;
            }
        }
    };

    auto FindY = [columns] (const std::string& signal) {
        for (size_t iter = 0; iter < columns.size(); iter++) {
            if (signal ==  columns[iter]) {
                return iter;
            }
        }
    };

    for (auto& transition : moore.transitions) {
        auto position = FindX(transition.startID) + FindY(transition.signal) * headers.size();
        cells.at(position) = {transition.endID};
    }

    return {headers, columns, cells};
}

std::vector<std::string> GetMealyHeaders(const AutomatonMoore& moore) {
    std::set<std::string> headers;

    for (const auto& state : moore.states) {
        headers.insert(state.name);
    }
    std::vector<std::string> sortedHeaders(headers.begin(), headers.end());
    std::sort(sortedHeaders.begin(), sortedHeaders.end());

    return sortedHeaders;
}

MealyTable Moore2Mealy(const AutomatonMoore& moore) {
    auto moreTable = GetMooreTable(moore);

    MealyTable mealyTable;
    mealyTable.headers = GetMealyHeaders(moore);
    mealyTable.columns = moreTable.columns;
    mealyTable.cells.resize(mealyTable.headers.size() * mealyTable.columns.size());

    auto cellsPerLine = mealyTable.headers.size();
    for (size_t y = 0; y < mealyTable.columns.size(); y++) {
        for (size_t x = 0; x < mealyTable.headers.size(); x++) {
            auto& targetCell = mealyTable.cells.at(cellsPerLine * y + x);

            targetCell.output = moreTable.headers.at(x).output;
            auto newNodeID = moreTable.cells.at(cellsPerLine * y + x).newNodeID;
            targetCell.endID = newNodeID;
        }
    }

    return mealyTable;
}