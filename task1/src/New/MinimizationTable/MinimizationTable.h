#pragma once
#include "../Table/Table.h"

struct MinimizationCellPayload {
    int toEquivalenceClass = -1;
};

struct MinimizationColumnPayload {
    int equivalenceClass = -1;
};

class MinimizationColumn : public TableColumn<MinimizationCellPayload> {};

class MinimizationTable : public Table<MinimizationColumnPayload, MinimizationCellPayload> {};