#include  "Print.h"
#include "../AutomatTables/AutomatTables.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

void Print(MealyTable& table) {
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

void Print(MooreTable& table) {
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

void PrintTable(MealyTable& table) {
    const auto columnIDs = table.GetColumnIDs();
    if (columnIDs.empty()) {
        std::cout << "type: mealy\n(empty)\n";
        return;
    }

    // Собираем и сортируем входы (строки)
    auto rowIDs = table.GetColumnByID(columnIDs.front()).column.GetRowIDs();
    std::sort(rowIDs.begin(), rowIDs.end());

    // Ширина колонок (примерно)
    const int colW = 14;
    const int rowW = 8;

    // Заголовок:   |  s0   |  s1   | ...
    std::cout << std::left << std::setw(rowW) << "";
    for (const auto& state : columnIDs) {
        std::cout << "| " << std::setw(colW - 2) << state;
    }
    std::cout << "|\n";

    // Разделитель
    std::cout << std::string(rowW, '-');
    for (size_t i = 0; i < columnIDs.size(); ++i)
        std::cout << "+" << std::string(colW - 1, '-');
    std::cout << "+\n";

    // Строки: вход |  next / y  |  next / y  | ...
    for (const auto& input : rowIDs) {
        std::cout << std::left << std::setw(rowW) << input;
        for (const auto& from : columnIDs) {
            const auto& cell = table.GetColumnByID(from).column.GetRowByID(input);
            std::string cellStr = cell.toColumnID + " / " + cell.outputSignal;
            std::cout << "| " << std::setw(colW - 2) << cellStr;
        }
        std::cout << "|\n";
    }

    // Строка классов эквивалентности
    std::cout << std::string(rowW, '-');
    for (size_t i = 0; i < columnIDs.size(); ++i)
        std::cout << "+" << std::string(colW - 1, '-');
    std::cout << "+\n";

    std::cout << std::left << std::setw(rowW) << "eq.class";
    for (const auto& state : columnIDs) {
        // Замени .equivalenceClass на реальное имя поля, если другое
        std::string eq = std::to_string(table.GetColumnByID(state).payload.equivalenceClass);
        std::cout << "| " << std::setw(colW - 2) << eq;
    }
    std::cout << "|\n";
}

void PrintTable(MooreTable& table) {
    const auto columnIDs = table.GetColumnIDs();
    if (columnIDs.empty()) {
        std::cout << "type: moore\n(empty)\n";
        return;
    }

    auto rowIDs = table.GetColumnByID(columnIDs.front()).column.GetRowIDs();
    std::sort(rowIDs.begin(), rowIDs.end());

    const int colW = 14;
    const int rowW = 8;

    // 1-я строка заголовка: общие имена состояний Moore
    std::cout << std::left << std::setw(rowW) << "";
    for (const auto& state : columnIDs) {
        std::cout << "| " << std::setw(colW - 2) << state;
    }
    std::cout << "|\n";

    // 2-я строка заголовка: аналог Mealy  stateID / Y
    std::cout << std::left << std::setw(rowW) << "";
    for (const auto& state : columnIDs) {
        const auto& payload = table.GetColumnByID(state).payload;
        std::string mealyLike = payload.stateID + " / " + payload.outputSignal;
        std::cout << "| " << std::setw(colW - 2) << mealyLike;
    }
    std::cout << "|\n";

    // Разделитель
    std::cout << std::string(rowW, '-');
    for (size_t i = 0; i < columnIDs.size(); ++i)
        std::cout << "+" << std::string(colW - 1, '-');
    std::cout << "+\n";

    // Переходы (только next-state, без Y — Y уже в состоянии)
    for (const auto& input : rowIDs) {
        std::cout << std::left << std::setw(rowW) << input;
        for (const auto& from : columnIDs) {
            const auto& cell = table.GetColumnByID(from).column.GetRowByID(input);
            std::cout << "| " << std::setw(colW - 2) << cell.toColumnID;
        }
        std::cout << "|\n";
    }

    // Строка классов эквивалентности
    std::cout << std::string(rowW, '-');
    for (size_t i = 0; i < columnIDs.size(); ++i)
        std::cout << "+" << std::string(colW - 1, '-');
    std::cout << "+\n";

    std::cout << std::left << std::setw(rowW) << "eq.class";
    for (const auto& state : columnIDs) {
        // Замени .equivalenceClass на реальное имя поля
        std::string eq = std::to_string(table.GetColumnByID(state).payload.equivalenceClass);
        std::cout << "| " << std::setw(colW - 2) << eq;
    }
    std::cout << "|\n";
}

void PrintTable(MinimizationTable& table) {
    const auto columnIDs = table.GetColumnIDs();
    if (columnIDs.empty()) {
        std::cout << "type: minimization\n(empty)\n";
        return;
    }

    // Собираем и сортируем входы (строки)
    auto rowIDs = table.GetColumnByID(columnIDs.front()).column.GetRowIDs();
    std::sort(rowIDs.begin(), rowIDs.end());

    const int colW = 12;
    const int rowW = 8;

    // Заголовок: имена колонок (состояния / классы)
    std::cout << std::left << std::setw(rowW) << "";
    for (const auto& colID : columnIDs) {
        std::cout << "| " << std::setw(colW - 2) << colID;
    }
    std::cout << "|\n";

    // Строка с equivalenceClass колонок
    std::cout << std::left << std::setw(rowW) << "eq.class";
    for (const auto& colID : columnIDs) {
        const auto& payload = table.GetColumnByID(colID).payload;
        std::cout << "| " << std::setw(colW - 2) << payload.equivalenceClass;
    }
    std::cout << "|\n";

    // Разделитель
    std::cout << std::string(rowW, '-');
    for (size_t i = 0; i < columnIDs.size(); ++i)
        std::cout << "+" << std::string(colW - 1, '-');
    std::cout << "+\n";

    // Строки переходов: toEquivalenceClass
    for (const auto& input : rowIDs) {
        std::cout << std::left << std::setw(rowW) << input;
        for (const auto& from : columnIDs) {
            const auto& cell = table.GetColumnByID(from).column.GetRowByID(input);
            std::cout << "| " << std::setw(colW - 2) << cell.toEquivalenceClass;
        }
        std::cout << "|\n";
    }
}