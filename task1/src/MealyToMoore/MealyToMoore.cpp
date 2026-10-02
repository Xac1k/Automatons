#include "MealyToMoore.h"
#include <set>

std::set<MealyCellPayload> GetUniqueStates(MealyTable& table) {
    const auto columnIDs = table.GetColumnIDs();
    const auto rowIDs = table.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();

    std::set<MealyCellPayload> mooreTableHeaders;
    for (const auto& columnID : columnIDs ) {
        for (const auto& rowID : rowIDs ) {
            auto column = table.GetColumnByID(columnID);
            auto row = column.column.GetRowByID(rowID);

            mooreTableHeaders.insert(row);
        }
    }

    return mooreTableHeaders;
}

std::map<std::string, MealyCellPayload> CreateNewNameOfStates(const std::set<MealyCellPayload>& mooreHeaders) {
    std::map<std::string, MealyCellPayload> associatedNameStates;
    size_t iter = 0;
    for (const auto& header : mooreHeaders) {
        associatedNameStates.emplace("Z" + std::to_string(iter++), header);
    }

    return associatedNameStates;
}

std::optional<std::string> GetNewNameByPayload(const std::map<std::string, MealyCellPayload>& newHeaders, const MealyCellPayload& payload) {
    for ( const auto& [newName, prevPayload] : newHeaders ) {
        if (prevPayload == payload) {
            return newName;
        }
    }
    return std::nullopt;
}

MooreTable CreateMooreTable(const std::map<std::string, MealyCellPayload>& newHeaders, MealyTable& table) {
    const auto columnIDs = table.GetColumnIDs();
    const auto rowIDs = table.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();

    MooreTable newTable;
    for (const auto& [newName, payload] : newHeaders ) {
        MooreTable::TableColumnData columnData;
        columnData.payload.outputSignal = payload.outputSignal;
        columnData.payload.stateID = payload.toColumnID;

        for (const auto& rowID : rowIDs ) {
            auto row = table.GetColumnByID(payload.toColumnID).column.GetRowByID(rowID);
            auto associatedName = GetNewNameByPayload(newHeaders, row);

            MooreCellPayload newPayload;
            newPayload.toColumnID = associatedName.value();
            columnData.column.AddNewRow(rowID, newPayload);
        }
        newTable.AddNewColumn(newName, columnData);
    }

    return newTable;
}

MooreTable Mealy2Moore(MealyTable& table) {
    const auto uniqueStates = GetUniqueStates(table);
    auto associatedNameStates = CreateNewNameOfStates(uniqueStates);

    return CreateMooreTable(associatedNameStates, table);
}
