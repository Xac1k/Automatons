#include "Minimize.h"
#include "../../New/MinimizationTable/MinimizationTable.h"
#include <stdexcept>
#include <algorithm>
#include <set>

namespace {
    constexpr int UNSET_EQUIVALENCE_CLASS = -1;

    bool MatchValuesInRows(MealyTable& mealyTable, const MealyTable::ColumnID& first, const MealyTable::ColumnID& second) {
        auto& firstColumn = mealyTable.GetColumnByID(first);
        auto& secondColumn = mealyTable.GetColumnByID(second);

        for (const auto& rowID: firstColumn.column.GetRowIDs()) {
            auto& firstCell = firstColumn.column.GetRowByID(rowID);
            auto& secondCell = secondColumn.column.GetRowByID(rowID);

            if (firstCell.outputSignal != secondCell.outputSignal) {
                return false;
            }
        }
        return true;
    }

    bool MatchValuesInRows(MinimizationTable& minimizationTable, const MinimizationTable::ColumnID& first, const MinimizationTable::ColumnID& second) {
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

    std::vector<std::vector<MealyTable::ColumnID>> SplitOntoSectorsOfEquivalenceClasses(MealyTable& mealyTable) {
        const auto columnIDs = mealyTable.GetColumnIDs();

        std::vector<MealyTable::ColumnID> treatedColumns;
        std::vector<std::vector<MealyTable::ColumnID>> sectors;
        auto FindInSectors = [&sectors](const MealyTable::ColumnID& columnID) -> std::vector<MealyTable::ColumnID>* {
            for (auto& equivalenceClass : sectors) {
                auto it = std::ranges::find(equivalenceClass, columnID);
                if (it != equivalenceClass.end()) {
                    return &equivalenceClass;
                }
            }
            return nullptr;
        };
        for (const auto& columnID: columnIDs) {
            auto it = std::ranges::find(treatedColumns, columnID);
            if (it != treatedColumns.end()) {
                continue;
            }

            for (const auto& otherColumnID: columnIDs) {
                if (columnID == otherColumnID) {
                    continue;
                }

                if (MatchValuesInRows(mealyTable, columnID, otherColumnID)) {
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
                    if (MatchValuesInRows(mealyTable, someColumnIDInSector, otherColumnID)) {
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
        }

        return sectors;
    }

    std::unordered_map<MealyTable::ColumnID, int> CreateTableOfInitialEquivalenceClasses(MealyTable& mealyTable) {
        const auto sectors = SplitOntoSectorsOfEquivalenceClasses(mealyTable);

        std::unordered_map<MealyTable::ColumnID, int> columnIDToEquivalenceClass;
        for (int equivalenceClass = 0; equivalenceClass < sectors.size(); ++equivalenceClass) {
            auto equivalenceClassIDs = sectors.at(equivalenceClass );
            for (const auto& columnID : equivalenceClassIDs) {
                columnIDToEquivalenceClass[columnID] = equivalenceClass + 1;
            }
        }

        return columnIDToEquivalenceClass;
    }

    MinimizationTable CreateEmptyMinimizationTable(MealyTable& mealyTable) {
        const auto columnIDs = mealyTable.GetColumnIDs();
        const auto rowIDs = mealyTable.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();
        auto columnIDToEquivalenceClass = CreateTableOfInitialEquivalenceClasses(mealyTable);

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

    bool UpdateValuesOfEquivalenceClassInCells(MinimizationTable& minimizationTable, MealyTable& mealyTable) {
        const auto columnIDs = mealyTable.GetColumnIDs();
        const auto rowIDs = mealyTable.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();
        bool updated = false;

        auto FindCellInMealy = [&mealyTable] (const MooreTable::ColumnID& columnID, const MinimizationColumn::RowID& rowID) {
            return &mealyTable.GetColumnByID(columnID).column.GetRowByID(rowID);
        };

        auto FindCellInMinimization = [&minimizationTable] (const MooreTable::ColumnID& columnID, const MinimizationColumn::RowID& rowID) {
            return &minimizationTable.GetColumnByID(columnID).column.GetRowByID(rowID);
        };

        for (const auto& columnID: columnIDs) {
            for (const auto& rowID: rowIDs) {
                const auto infoCell = FindCellInMealy(columnID, rowID);
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

    std::vector<std::string> GetOtherColumnIDsOfEquivalenceClass(MinimizationTable& minimizationTable, const MealyTable::ColumnID& columnID) {
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

    MealyTable CreateMinimizedTable(MinimizationTable& minimizationTable, MealyTable& table) {
        auto columnIDs = minimizationTable.GetColumnIDs();
        auto rowIDs = minimizationTable.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();

        std::unordered_map<int, std::vector<MealyTable::ColumnID>> uniqueEquivalenceClasses;
        for (const auto& columnID : columnIDs) {
            auto column = minimizationTable.GetColumnByID(columnID);
            auto it = uniqueEquivalenceClasses.find(column.payload.equivalenceClass);
            if (it != uniqueEquivalenceClasses.end()) {
                it->second.push_back(columnID);
            } else {
                uniqueEquivalenceClasses[column.payload.equivalenceClass] = {columnID};
            }
        }

        MealyTable resultTable;
        for (const auto& [equivalenceClass, instances] : uniqueEquivalenceClasses) {
            auto columnOfEquivalenceClass = GetInstanceOfEquivalenceClass(minimizationTable, equivalenceClass).value();

            MealyTable::TableColumnData columnData;
            columnData.payload.equivalenceClass = equivalenceClass;
            for (const auto& rowID: rowIDs) {
                MealyCellPayload mealyCellPayload;
                mealyCellPayload.toColumnID = std::to_string(columnOfEquivalenceClass.column.GetRowByID(rowID).toEquivalenceClass);
                mealyCellPayload.outputSignal = table.GetColumnByID(instances.at(0)).column.GetRowByID(rowID).outputSignal;

                columnData.column.AddNewRow(rowID, mealyCellPayload);
            }

            resultTable.AddNewColumn(std::to_string(equivalenceClass), columnData);
        }

        return resultTable;
    }
}

MealyTable Minimize(MealyTable table) {
    auto minimizationTable = CreateEmptyMinimizationTable(table);
    while (UpdateValuesOfEquivalenceClassInCells(minimizationTable, table)) {
        CheckEquivalenceMatrixAndSplit(minimizationTable);
    }

    auto minimizedTable = CreateMinimizedTable(minimizationTable, table);
    return minimizedTable;
}