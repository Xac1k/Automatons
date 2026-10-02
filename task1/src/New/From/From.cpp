#include <set>
#include <string>
#include "From.h"

MealyTable From(const AutomatonMealy& automaton) {
    MealyTable table;

    // 1. Собираем множества состояний и входных сигналов
    std::set<std::string> stateSet;
    std::set<std::string> signalSet;

    stateSet.insert(automaton.startStateID);
    for (const auto& t : automaton.transitions) {
        stateSet.insert(t.startID);
        stateSet.insert(t.endID);
        signalSet.insert(t.signal);
    }

    // 2. Создаём столбцы — по одному на каждое состояние
    for (const auto& state : stateSet) {
        MealyTable::TableColumnData colData;
        colData.payload.equivalenceClass = 0;
        table.AddNewColumn(state, colData);
    }

    // 3. Заполняем таблицу: строки — входные сигналы, столбцы — состояния
    //    В ячейке: состояние перехода + выходной сигнал
    for (const auto& signal : signalSet) {
        for (const auto& state : stateSet) {
            MealyCellPayload cell;
            bool found = false;

            for (const auto& t : automaton.transitions) {
                if (t.startID == state && t.signal == signal) {
                    cell.toColumnID = t.endID;       // состояние, в которое переходим
                    cell.outputSignal = t.output;    // выходной сигнал
                    found = true;
                    break;
                }
            }

            if (!found) {
                cell.toColumnID = "";
                cell.outputSignal = "";
            }

            table.GetColumnByID(state).column.AddNewRow(signal, cell);
        }
    }

    return table;
}


MooreTable From(const AutomatonMoore& automaton) {
    MooreTable table;

    // 1. Собираем множество входных сигналов
    std::set<std::string> signalSet;
    for (const auto& t : automaton.transitions) {
        signalSet.insert(t.signal);
    }

    // 2. Создаём столбцы — по одному на каждое состояние.
    //    В payload храним stateID (первоначальное состояние) и выходной сигнал.
    //    Ключ столбца (ColumnID) — общее название состояния.
    for (const auto& state : automaton.states) {
        MooreTable::TableColumnData colData;
        colData.payload.equivalenceClass = 0;
        colData.payload.stateID = state.stateID;
        colData.payload.outputSignal = state.output;
        table.AddNewColumn(state.stateID, colData);
    }

    // 3. Заполняем таблицу: строки — входные сигналы, столбцы — состояния.
    //    В ячейке — состояние перехода (toColumnID).
    for (const auto& signal : signalSet) {
        for (const auto& state : automaton.states) {
            MooreCellPayload cell;
            bool found = false;

            for (const auto& t : automaton.transitions) {
                if (t.startID == state.stateID && t.signal == signal) {
                    cell.toColumnID = t.endID;
                    found = true;
                    break;
                }
            }

            if (!found) {
                cell.toColumnID = "";
            }

            table.GetColumnByID(state.stateID).column.AddNewRow(signal, cell);
        }
    }

    return table;
}