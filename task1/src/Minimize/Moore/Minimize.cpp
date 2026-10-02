#include <stdexcept>
#include <algorithm>
#include "Minimize.h"
#include "../../New/MinimizationTable/MinimizationTable.h"
namespace {
    constexpr int UNSET_EQUIVALENCE_CLASS = -1;

    std::unordered_map<MooreTable::ColumnID, int> CreateTableOfInitialEquivalenceClasses(MooreTable& mooreTable) {
        const auto columnIDs = mooreTable.GetColumnIDs();

        //Определяем начальные классы эквивалетности
        std::unordered_map<MooreTable::OutputSignal, std::vector<MooreTable::ColumnID>> uniqueOutputsSignal;
        std::unordered_map<MooreTable::OutputSignal, int> signalToEquivalenceClass;
        int equivalenceClassIter = 0;
        for (const auto& columnID : columnIDs) {
            const auto outputSignal = mooreTable.GetColumnByID(columnID).payload.outputSignal;
            auto it = uniqueOutputsSignal.find(outputSignal);
            if (it != uniqueOutputsSignal.end()) {
                it->second.push_back(columnID);
            } else {
                uniqueOutputsSignal[outputSignal] = {columnID};
                signalToEquivalenceClass[outputSignal] = equivalenceClassIter++;
            }
        }

        //Строим обратную таблицу к классам эквивалентности
        std::unordered_map<MooreTable::ColumnID, int> columnIDToEquivalenceClass;
        for (const auto& [outputSignal, equivalenceClassIDs] : uniqueOutputsSignal) {
            for (const auto& columnID : equivalenceClassIDs) {
                columnIDToEquivalenceClass[columnID] = signalToEquivalenceClass[outputSignal];
            }
        }

        return columnIDToEquivalenceClass;
    }

    MinimizationTable CreateEmptyMinimizationTable(MooreTable& mooreTable) {
        const auto columnIDs = mooreTable.GetColumnIDs();
        const auto rowIDs = mooreTable.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();
        auto columnIDToEquivalenceClass = CreateTableOfInitialEquivalenceClasses(mooreTable);

        MinimizationTable minimizationTable;
        for (const auto& columnID: columnIDs) {
            MinimizationColumn minimizationColumn;
            for (const auto& rowID: rowIDs) {
                minimizationColumn.AddNewRow(rowID, {UNSET_EQUIVALENCE_CLASS});
            }

            MinimizationTable::TableColumnData columnData;
            columnData.column = minimizationColumn;
            columnData.payload.equivalenceClass = columnIDToEquivalenceClass[columnID];

            minimizationTable.AddNewColumn(columnID, columnData);
        }

        return minimizationTable;
    }

    bool UpdateValuesOfEquivalenceClassInCells(MinimizationTable& minimizationTable, MooreTable& mooreTable) {
        const auto columnIDs = mooreTable.GetColumnIDs();
        const auto rowIDs = mooreTable.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();
        bool updated = false;

        auto FindCellInMoore = [&mooreTable] (const MooreTable::ColumnID& columnID, const MinimizationColumn::RowID& rowID) {
            return &mooreTable.GetColumnByID(columnID).column.GetRowByID(rowID);
        };

        auto FindCellInMinimization = [&minimizationTable] (const MooreTable::ColumnID& columnID, const MinimizationColumn::RowID& rowID) {
            return &minimizationTable.GetColumnByID(columnID).column.GetRowByID(rowID);
        };

        for (const auto& columnID: columnIDs) {
            for (const auto& rowID: rowIDs) {
                const auto infoCell = FindCellInMoore(columnID, rowID);
                const auto targetCell = FindCellInMinimization(columnID, rowID);
                auto targetEquivalenceClass = minimizationTable.GetColumnByID(infoCell->toColumnID).payload.equivalenceClass;
                if (targetCell->toEquivalenceClass != targetEquivalenceClass) {
                    updated = true;
                }
                targetCell->toEquivalenceClass = targetEquivalenceClass;
            }
        }

        return updated;
    }

    bool MatchValuesInRows(MinimizationTable& minimizationTable, const MooreTable::ColumnID& first, const MooreTable::ColumnID& second) {
        auto& firstColumn = minimizationTable.GetColumnByID(first);
        auto& secondColumn = minimizationTable.GetColumnByID(second);

        for (const auto& rowID: firstColumn.column.GetRowIDs()) {
            auto& firstCell = firstColumn.column.GetRowByID(rowID);
            auto& secondCell = secondColumn.column.GetRowByID(rowID);

            if (firstCell.toEquivalenceClass != secondCell.toEquivalenceClass) {
                return false;
            }
        }
        return true;
    }

    std::vector<std::string> GetOtherColumnIDsOfEquivalenceClass(MinimizationTable& minimizationTable, const MooreTable::ColumnID columnID) {
        auto columnIDs = minimizationTable.GetColumnIDs();
        std::vector<MooreTable::ColumnID> othersColumnIDs;
        auto targetColumn = minimizationTable.GetColumnByID(columnID);

        for (const auto& comparisonColumnID : columnIDs) {
            if (comparisonColumnID == columnID) {
                continue;
            }
            auto column = minimizationTable.GetColumnByID(comparisonColumnID);
            if (targetColumn.payload.equivalenceClass == column.payload.equivalenceClass) {
                othersColumnIDs.push_back(comparisonColumnID);
            }
        }

        return othersColumnIDs;
    }

    int GetMaxEquivalenceClassValue(MinimizationTable& minimizationTable) {
        auto columnIDs = minimizationTable.GetColumnIDs();
        int equivalenceClass = -1;

        for (const auto& columnID : columnIDs) {
            auto column = minimizationTable.GetColumnByID(columnID);
            if (equivalenceClass < column.payload.equivalenceClass) {
                equivalenceClass = column.payload.equivalenceClass;
            }
        }

        return equivalenceClass;
    }

    void SplitSectorsInEquivalenceClasses(MinimizationTable& minimizationTable, const std::vector<std::vector<MooreTable::ColumnID>>& sectors) {
        if (sectors.size() > 1) {
            auto previousMaxEquivalenceClassValue = GetMaxEquivalenceClassValue(minimizationTable);
            for (size_t i = 1; i < sectors.size(); i++) {
                auto sector = sectors.at(i);
                for (const auto& columnIDInSector : sector) {
                    auto& column = minimizationTable.GetColumnByID(columnIDInSector);
                    column.payload.equivalenceClass = previousMaxEquivalenceClassValue + 1;
                }
                previousMaxEquivalenceClassValue++;
            }
        }
    }

    void CheckEquivalenceMatrixAndSplit(MinimizationTable& minimizationTable) {
        auto columnIDs = minimizationTable.GetColumnIDs();

        std::vector<MooreTable::ColumnID> treatedColumns;
        for (auto columnID: columnIDs) {
            //Проверка обработанности колонки. Если уже куда-то попала, то можно её не обрабатывать
            auto it = std::ranges::find(treatedColumns, columnID);
            if (it != treatedColumns.end()) {
                continue;
            }

            treatedColumns.push_back(columnID);
            auto othersInClass = GetOtherColumnIDsOfEquivalenceClass(minimizationTable, columnID);

            std::vector<std::vector<MooreTable::ColumnID>> sectors = {{columnID}};
            auto FindInSectors = [&sectors](const MooreTable::ColumnID& columnID) -> std::vector<MooreTable::ColumnID>* {
                for (auto& equivalenceClass : sectors) {
                    auto it = std::ranges::find(equivalenceClass, columnID);
                    if (it != equivalenceClass.end()) {
                        return &equivalenceClass;
                    }
                }
                return nullptr;
            };

            //Проверка метчинга и деление
            for (const auto& otherColumnID: othersInClass) {
                if (MatchValuesInRows(minimizationTable, columnID, otherColumnID)) {
                    const auto equivalenceClass = FindInSectors(columnID);
                    if (equivalenceClass != nullptr) {
                        equivalenceClass->push_back(otherColumnID);
                        treatedColumns.push_back(otherColumnID);
                        continue;
                    }
                    sectors.push_back({otherColumnID, columnID});
                    treatedColumns.push_back(otherColumnID);
                    treatedColumns.push_back(columnID);
                    continue;
                }
                bool pushed = false;
                for (auto sector : sectors) {
                    if (sector.empty()) continue;
                    const auto& someColumnIDInSector = sector.at(0);
                    if (MatchValuesInRows(minimizationTable, someColumnIDInSector, otherColumnID)) {
                        const auto equivalenceClass = FindInSectors(someColumnIDInSector);
                        equivalenceClass->push_back(otherColumnID);
                        treatedColumns.push_back(otherColumnID);
                        pushed = true;
                    }
                }
                if (!pushed) {
                    sectors.push_back({otherColumnID});
                    treatedColumns.push_back(otherColumnID);
                }
            }

            SplitSectorsInEquivalenceClasses(minimizationTable, sectors);
        }
    }

    std::optional<MinimizationTable::TableColumnData> GetInstanceOfEquivalenceClass(MinimizationTable& minimizationTable, int equivalenceClass) {
        auto columnIDs = minimizationTable.GetColumnIDs();
        for (const auto& columnID: columnIDs) {
            auto column = minimizationTable.GetColumnByID(columnID);
            if (column.payload.equivalenceClass == equivalenceClass) {
                return column;
            }
        }

        return std::nullopt;
    }

    MooreTable CreateMinimizedTable(MinimizationTable& minimizationTable, MooreTable& table) {
        auto columnIDs = minimizationTable.GetColumnIDs();
        auto rowIDs = minimizationTable.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();

        std::unordered_map<int, std::vector<MooreTable::ColumnID>> uniqueEquivalenceClasses;
        for (const auto& columnID : columnIDs) {
            auto column = minimizationTable.GetColumnByID(columnID);
            auto it = uniqueEquivalenceClasses.find(column.payload.equivalenceClass);
            if (it != uniqueEquivalenceClasses.end()) {
                it->second.push_back(columnID);
            } else {
                uniqueEquivalenceClasses[column.payload.equivalenceClass] = {columnID};
            }
        }

        MooreTable resultTable;
        for (const auto& [equivalenceClass, instances] : uniqueEquivalenceClasses) {
            auto columnOfEquivalenceClass = GetInstanceOfEquivalenceClass(minimizationTable, equivalenceClass).value();

            MooreTable::TableColumnData columnData;
            columnData.payload.stateID = std::to_string(equivalenceClass);
            columnData.payload.outputSignal = table.GetColumnByID(instances.at(0)).payload.outputSignal;
            columnData.payload.equivalenceClass = equivalenceClass;
            for (const auto& rowID: rowIDs) {
                MooreCellPayload mealyCellPayload;
                mealyCellPayload.toColumnID = std::to_string(columnOfEquivalenceClass.column.GetRowByID(rowID).toEquivalenceClass);

                columnData.column.AddNewRow(rowID, mealyCellPayload);
            }

            resultTable.AddNewColumn(std::to_string(equivalenceClass), columnData);
        }

        return resultTable;
    }
}

MooreTable Minimize(MooreTable table) {
    const auto columnIDs = table.GetColumnIDs();
    if (columnIDs.empty()) {
        throw std::logic_error("Can't optimize the air in columns.");
    }
    const auto rowIDs = table.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();
    if (rowIDs.empty()) {
        throw std::logic_error("Can't optimize the air in rows.");
    }

    auto minimizationTable = CreateEmptyMinimizationTable(table);
    while (UpdateValuesOfEquivalenceClassInCells(minimizationTable, table)) {
        CheckEquivalenceMatrixAndSplit(minimizationTable);
    }

    auto minimizedTable = CreateMinimizedTable(minimizationTable, table);
    return minimizedTable;
};