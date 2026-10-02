#pragma once
#include "../Table/Table.h"

using OutputSignal = std::string;

struct MealyHeaderPayload {
    unsigned long equivalenceClass;
};

struct MealyCellPayload {
    std::string toColumnID;
    OutputSignal outputSignal;

    auto operator<=>(const MealyCellPayload& other) const {
        return std::tie(toColumnID, outputSignal) <=> std::tie(other.toColumnID, other.outputSignal);
    }

    bool operator==(const MealyCellPayload& payload) const = default;
};

class MealyTable : public Table<MealyHeaderPayload, MealyCellPayload> {
public:
    using OutputSignal = OutputSignal;
};

struct MooreHeaderPayload {
    unsigned long equivalenceClass;
    std::string stateID;
    OutputSignal outputSignal;
};

struct MooreCellPayload {
    std::string toColumnID;
};

class MooreTable: public Table<MooreHeaderPayload, MooreCellPayload> {
public:
    using OutputSignal = OutputSignal;
};