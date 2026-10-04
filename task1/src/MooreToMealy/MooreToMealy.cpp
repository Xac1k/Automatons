#include "MooreToMealy.h"

MealyTable Moore2Mealy(MooreTable& table) {
    const auto columnIDs = table.GetColumnIDs();
    const auto rowIDs = table.GetColumnByID(columnIDs.at(0)).column.GetRowIDs();

    MealyTable mealyTable;
    for (const auto& columnID : columnIDs) {

        MealyTable::TableColumnData columnData;
        auto fromColumn = table.GetColumnByID(columnID);

        for (const auto& rowID : rowIDs) {
            MealyCellPayload cellPayload;
            auto fromRow = fromColumn.column.GetRowByID(rowID);
            auto targetColumn = table.GetColumnByID(fromRow.toColumnID);

            cellPayload.outputSignal = targetColumn.payload.outputSignal;
            cellPayload.toColumnID = fromRow.toColumnID;

            columnData.column.AddNewRow(rowID, cellPayload);
        }
        mealyTable.AddNewColumn(columnID, columnData);
    }

    return mealyTable;
}