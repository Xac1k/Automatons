#include <iostream>
#include <ostream>

#include "src/Minimize/Moore/Minimize.h"
#include "src/Minimize/Mealy/Minimize.h"
#include "src/MealyToMoore/MealyToMoore.h"
#include "src/MooreToMealy/MooreToMealy.h"
#include "src/New/DeterminizeMoore/DeterminizeMoore.h"
#include "src/New/From/From.h"
#include "src/New/Print/Print.h"
#include "src/Old/Parse/Parse.h"

MooreTable CreateTestMooreTable() {
    MooreTable table;

    {
        MooreCellPayload cellData1;
        cellData1.toColumnID = "M5";

        MooreCellPayload cellData2;
        cellData2.toColumnID = "M7";

        MooreTable::TableColumnData columnData;
        columnData.payload.outputSignal = "Y1";
        columnData.payload.stateID = "A1";

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M1", columnData);
    }

    {
        MooreCellPayload cellData1;
        cellData1.toColumnID = "M4";

        MooreCellPayload cellData2;
        cellData2.toColumnID = "M1";

        MooreTable::TableColumnData columnData;
        columnData.payload.outputSignal = "Y2";
        columnData.payload.stateID = "A2";

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M2", columnData);
    }

    {
        MooreCellPayload cellData1;
        cellData1.toColumnID = "M5";

        MooreCellPayload cellData2;
        cellData2.toColumnID = "M4";

        MooreTable::TableColumnData columnData;
        columnData.payload.outputSignal = "Y3";
        columnData.payload.stateID = "A3";

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M3", columnData);
    }

    {
        MooreCellPayload cellData1;
        cellData1.toColumnID = "M3";

        MooreCellPayload cellData2;
        cellData2.toColumnID = "M2";

        MooreTable::TableColumnData columnData;
        columnData.payload.outputSignal = "Y1";
        columnData.payload.stateID = "A4";

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M4", columnData);
    }

    {
        MooreCellPayload cellData1;
        cellData1.toColumnID = "M4";

        MooreCellPayload cellData2;
        cellData2.toColumnID = "M1";

        MooreTable::TableColumnData columnData;
        columnData.payload.outputSignal = "Y2";
        columnData.payload.stateID = "A5";

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M5", columnData);
    }

    {
        MooreCellPayload cellData1;
        cellData1.toColumnID = "M2";

        MooreCellPayload cellData2;
        cellData2.toColumnID = "M3";

        MooreTable::TableColumnData columnData;
        columnData.payload.outputSignal = "Y2";
        columnData.payload.stateID = "A6";

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M6", columnData);
    }

    {
        MooreCellPayload cellData1;
        cellData1.toColumnID = "M5";

        MooreCellPayload cellData2;
        cellData2.toColumnID = "M4";

        MooreTable::TableColumnData columnData;
        columnData.payload.outputSignal = "Y3";
        columnData.payload.stateID = "A7";

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M7", columnData);
    }

    return table;
}

MealyTable CreateTestMealyTable() {
    MealyTable table;


    {
        MealyCellPayload cellData1;
        cellData1.toColumnID = "M3";
        cellData1.outputSignal = "Y1";

        MealyCellPayload cellData2;
        cellData2.toColumnID = "M5";
        cellData2.outputSignal = "Y1";

        MealyTable::TableColumnData columnData;

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M1", columnData);
    }

    {
        MealyCellPayload cellData1;
        cellData1.toColumnID = "M4";
        cellData1.outputSignal = "Y1";

        MealyCellPayload cellData2;
        cellData2.toColumnID = "M6";
        cellData2.outputSignal = "Y1";

        MealyTable::TableColumnData columnData;

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M2", columnData);
    }

    {
        MealyCellPayload cellData1;
        cellData1.toColumnID = "M3";
        cellData1.outputSignal = "Y1";

        MealyCellPayload cellData2;
        cellData2.toColumnID = "M5";
        cellData2.outputSignal = "Y2";

        MealyTable::TableColumnData columnData;

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M3", columnData);
    }

    {
        MealyCellPayload cellData1;
        cellData1.toColumnID = "M4";
        cellData1.outputSignal = "Y1";

        MealyCellPayload cellData2;
        cellData2.toColumnID = "M6";
        cellData2.outputSignal = "Y2";

        MealyTable::TableColumnData columnData;

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M4", columnData);
    }

    {
        MealyCellPayload cellData1;
        cellData1.toColumnID = "M1";
        cellData1.outputSignal = "Y1";

        MealyCellPayload cellData2;
        cellData2.toColumnID = "M3";
        cellData2.outputSignal = "Y1";

        MealyTable::TableColumnData columnData;

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M5", columnData);
    }

    {
        MealyCellPayload cellData1;
        cellData1.toColumnID = "M2";
        cellData1.outputSignal = "Y1";

        MealyCellPayload cellData2;
        cellData2.toColumnID = "M4";
        cellData2.outputSignal = "Y1";

        MealyTable::TableColumnData columnData;

        columnData.column.AddNewRow("1", cellData1);
        columnData.column.AddNewRow("2", cellData2);

        table.AddNewColumn("M6", columnData);
    }

    return table;
}

int main() {
    auto moore = MooreParse(R"(type: moore
start: q0

states:
q0 | q0 / OUTPUT
q1 | q1 / OUTPUT
q2 | q2 / OUTPUT

transitions:
q0 q0 A
q0 q1 EMPTY
q1 q1 B
q1 q2 EMPTY
q2 q2 C)");

    auto mooreTable = From(moore);
    auto determinized = DeterminizeMoore(mooreTable, "q0");

    Print(determinized);
}