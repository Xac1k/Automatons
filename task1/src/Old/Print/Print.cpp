#include "Print.h"

#include <iomanip>

void Print(const AutomatonMoore& automaton, std::ostream& os) {
    os << "type: moore\n";
    os << "start: " << automaton.startStateID << "\n";
    os << "\n";

    os << "states:\n";
    for (const auto& s : automaton.states) {
        os << s.stateID << " | " << s.name << " / " << s.output << "\n";
    }
    os << "\n";

    os << "transitions:\n";
    for (const auto& t : automaton.transitions) {
        os << t.startID << " " << t.toID << " " << t.signal << "\n";
    }
}

void Print(const AutomatonMealy& automaton, std::ostream& os) {
    os << "type: mealy\n";
    os << "start: " << automaton.startStateID << "\n";
    os << "\n";

    os << "transitions:\n";
    for (const auto& t : automaton.transitions) {
        os << t.startID << " "
           << t.endID   << " "
           << t.signal  << " / "
           << t.output  << "\n";
    }
}

void Print(const MealyTable& table, std::ostream& out) {
    if (table.headers.empty() || table.columns.empty()) {
        out << "(empty Mealy table)\n";
        return;
    }

    const size_t nStates  = table.headers.size();
    const size_t nSignals = table.columns.size();

    // ---------- 1. Подготовка строк ячеек ----------
    // cellStrings[y][x] = "endID / output"  или  "-"
    std::vector<std::vector<std::string>> cellStrings(nSignals, std::vector<std::string>(nStates));

    size_t maxCellWidth = 1;
    for (size_t y = 0; y < nSignals; ++y) {
        for (size_t x = 0; x < nStates; ++x) {
            const MealyCell& c = table.cells[y * nStates + x];
            std::string s;
            if (c.endID.empty()) {
                s = "-";
            } else {
                s = c.endID + " / " + c.output;
            }
            cellStrings[y][x] = s;
            maxCellWidth = std::max(maxCellWidth, s.size());
        }
    }

    // ---------- 2. Ширины столбцов ----------
    size_t maxHeaderWidth = 0;
    for (const auto& h : table.headers)
        maxHeaderWidth = std::max(maxHeaderWidth, h.size());

    // ширина первого столбца (имена состояний)
    const size_t col0Width = std::max(maxHeaderWidth, size_t(1));

    // ширины остальных столбцов (по сигналам)
    std::vector<size_t> colWidths(nSignals);
    for (size_t y = 0; y < nSignals; ++y) {
        size_t w = table.columns[y].size();
        for (size_t x = 0; x < nStates; ++x)
            w = std::max(w, cellStrings[y][x].size());
        colWidths[y] = std::max(w, size_t(3));          // минимум 3 символа
    }

    // ---------- 3. Вспомогательные лямбды ----------
    auto printSep = [&]() {
        out << '+' << std::string(col0Width + 2, '-');
        for (size_t y = 0; y < nSignals; ++y)
            out << '+' << std::string(colWidths[y] + 2, '-');
        out << "+\n";
    };

    auto printCentered = [&](const std::string& s, size_t width) {
        size_t pad = width > s.size() ? width - s.size() : 0;
        size_t left  = pad / 2;
        size_t right = pad - left;
        out << std::string(left, ' ') << s << std::string(right, ' ');
    };

    // ---------- 4. Печать ----------
    printSep();

    // Заголовок: пустая ячейка + названия сигналов
    out << "| " << std::setw(static_cast<int>(col0Width)) << std::left << "" << " |";
    for (size_t y = 0; y < nSignals; ++y) {
        out << ' ';
        printCentered(table.columns[y], colWidths[y]);
        out << " |";
    }
    out << '\n';
    printSep();

    // Строки состояний
    for (size_t x = 0; x < nStates; ++x) {
        // имя состояния
        out << "| " << std::setw(static_cast<int>(col0Width)) << std::left
            << table.headers[x] << " |";

        // ячейки
        for (size_t y = 0; y < nSignals; ++y) {
            out << ' ';
            printCentered(cellStrings[y][x], colWidths[y]);
            out << " |";
        }
        out << '\n';
        printSep();
    }
}

void Print(const MooreTable& table, std::ostream& out)
{
    if (table.headers.empty() || table.columns.empty()) {
        out << "(empty Moore table)\n";
        return;
    }

    const size_t nStates  = table.headers.size();
    const size_t nSignals = table.columns.size();

    // ---------- 1. Строки для заголовков состояний ----------
    // Показываем как: newNodeID / output
    std::vector<std::string> headerStrings(nStates);
    size_t maxHeaderWidth = 0;
    for (size_t i = 0; i < nStates; ++i) {
        const auto& h = table.headers[i];
        std::string s = h.newNodeID + " / " + h.output;
        headerStrings[i] = s;
        maxHeaderWidth = std::max(maxHeaderWidth, s.size());
    }

    // ---------- 2. Строки ячеек ----------
    std::vector<std::vector<std::string>> cellStrings(nSignals, std::vector<std::string>(nStates));
    size_t maxCellWidth = 1;

    for (size_t y = 0; y < nSignals; ++y) {
        for (size_t x = 0; x < nStates; ++x) {
            const MooreCell& c = table.cells[y * nStates + x];
            std::string s = c.newNodeID.empty() ? "-" : c.newNodeID;
            cellStrings[y][x] = s;
            maxCellWidth = std::max(maxCellWidth, s.size());
        }
    }

    // ---------- 3. Ширины столбцов ----------
    // Первый столбец — подписи сигналов (или пустой)
    size_t col0Width = 0;
    for (const auto& col : table.columns)
        col0Width = std::max(col0Width, col.size());
    col0Width = std::max(col0Width, size_t(1));

    // Ширины столбцов состояний
    std::vector<size_t> colWidths(nStates);
    for (size_t x = 0; x < nStates; ++x) {
        size_t w = headerStrings[x].size();
        for (size_t y = 0; y < nSignals; ++y)
            w = std::max(w, cellStrings[y][x].size());
        colWidths[x] = std::max(w, size_t(3));
    }

    // ---------- 4. Вспомогательные функции ----------
    auto printSep = [&]() {
        out << '+' << std::string(col0Width + 2, '-');
        for (size_t x = 0; x < nStates; ++x)
            out << '+' << std::string(colWidths[x] + 2, '-');
        out << "+\n";
    };

    auto printCentered = [&](const std::string& s, size_t width) {
        size_t pad = width > s.size() ? width - s.size() : 0;
        size_t left  = pad / 2;
        size_t right = pad - left;
        out << std::string(left, ' ') << s << std::string(right, ' ');
    };

    // ---------- 5. Печать ----------
    printSep();

    // Верхняя строка: пустая ячейка + заголовки состояний (newNodeID / output)
    out << "| " << std::setw(static_cast<int>(col0Width)) << std::left << "" << " |";
    for (size_t x = 0; x < nStates; ++x) {
        out << ' ';
        printCentered(headerStrings[x], colWidths[x]);
        out << " |";
    }
    out << '\n';
    printSep();

    // Строки по сигналам
    for (size_t y = 0; y < nSignals; ++y) {
        // название сигнала
        out << "| " << std::setw(static_cast<int>(col0Width)) << std::left
            << table.columns[y] << " |";

        // ячейки (next state)
        for (size_t x = 0; x < nStates; ++x) {
            out << ' ';
            printCentered(cellStrings[y][x], colWidths[x]);
            out << " |";
        }
        out << '\n';
        printSep();
    }
}