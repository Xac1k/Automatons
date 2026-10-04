#include <iostream>
#include <ostream>

#include "src/Minimize/Moore/Minimize.h"
#include "src/Minimize/Mealy/Minimize.h"
#include "src/MealyToMoore/MealyToMoore.h"
#include "src/MooreToMealy/MooreToMealy.h"
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
    auto mealy = MealyParse(R"(type: mealy
start: 1

transitions:
1 10 Z1 / Y1
1 5 Z2 / Y2
2 12 Z1 / Y1
2 8 Z2 / Y2
3 5 Z1 / Y2
3 6 Z2 / Y1
4 7 Z1 / Y2
4 11 Z2 / Y1
5 3 Z1 / Y1
5 9 Z2 / Y2
6 7 Z1 / Y2
6 11 Z2 / Y1
7 3 Z1 / Y1
7 6 Z2 / Y2
8 10 Z1 / Y1
8 4 Z2 / Y2
9 7 Z1 / Y2
9 6 Z2 / Y1
10 1 Z1 / Y2
10 8 Z2 / Y1
11 5 Z1 / Y2
11 9 Z2 / Y1
12 2 Z1 / Y2
12 8 Z2 / Y1)");

    auto mealyTable = From(mealy);

    auto minimizedMealy = Minimize(mealyTable);

    auto mooreTable = Mealy2Moore(mealyTable);
    std::cout << "MealyTable -> MooreTable" << std::endl;
    PrintTable(mooreTable);

    auto minimizedMoore = Minimize(mooreTable);
    std::cout << "\n\n\nMealyTable -> MooreTable -> MinimizedMooreTable" << std::endl;
    PrintTable(minimizedMoore);

    auto mealyTable2 = Moore2Mealy(minimizedMoore);
    auto minimizedMealy2 = Minimize(mealyTable2);

    std::cout << "\n\n\nMealyTable -> MinimizedMealyTable" << std::endl;
    PrintTable(minimizedMealy);
    std::cout << "\n\n\nMealyTable -> MooreTable -> MinimizedMooreTable -> MealyTable2 -> MinimizedMealyTable2" << std::endl;
    PrintTable(minimizedMealy2);
}