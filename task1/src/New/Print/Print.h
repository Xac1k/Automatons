#pragma once
#include "../AutomatTables/AutomatTables.h"

#include <algorithm>
#include <iostream>
#include <vector>

inline void Print(MealyTable& table) {
    const auto columnIDs = table.GetColumnIDs();
    if (columnIDs.empty()) {
        std::cout << "type: mealy\nstart: \ntransitions:\n";
        return;
    }

    std::cout << "type: mealy\n";
    std::cout << "start: " << columnIDs.front() << "\n";

    std::cout << "\n";
    std::cout << "transitions:\n";

    // Берём порядок входов из первого столбца и сортируем для детерминированного вывода
    auto rowIDs = table.GetColumnByID(columnIDs.front()).column.GetRowIDs();
    std::sort(rowIDs.begin(), rowIDs.end());

    for (const auto& from : columnIDs) {
        auto& col = table.GetColumnByID(from);
        for (const auto& input : rowIDs) {
            const auto& cell = col.column.GetRowByID(input);
            std::cout << from << " " << cell.toColumnID << " "
                      << input << " / " << cell.outputSignal << "\n";
        }
    }
}

inline void Print(MooreTable& table) {
    const auto columnIDs = table.GetColumnIDs();
    if (columnIDs.empty()) {
        std::cout << "type: moore\nstart: \nstates:\ntransitions:\n";
        return;
    }

    std::cout << "type: moore\n";
    std::cout << "start: " << columnIDs.front() << "\n";
    std::cout << "\n";
    std::cout << "states:\n";

    for (const auto& state : columnIDs) {
        const auto& payload = table.GetColumnByID(state).payload;
        std::cout << state << " | " << payload.stateID << " / " << payload.outputSignal << "\n";
    }

    std::cout << "\n";
    std::cout << "transitions:\n";

    // Берём порядок входов из первого столбца и сортируем
    auto rowIDs = table.GetColumnByID(columnIDs.front()).column.GetRowIDs();
    std::sort(rowIDs.begin(), rowIDs.end());

    for (const auto& from : columnIDs) {
        auto& col = table.GetColumnByID(from);
        for (const auto& input : rowIDs) {
            const auto& cell = col.column.GetRowByID(input);
            std::cout << from << " " << cell.toColumnID << " " << input << "\n";
        }
    }
}
