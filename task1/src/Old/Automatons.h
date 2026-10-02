#pragma once
#include <string>
#include <vector>

struct AutomatonMoore {
    struct State {
        std::string stateID;
        std::string output;
        std::string name;

        auto operator<=>(const State& other) const {
            return std::tie(stateID, output) <=> std::tie(other.stateID, other.output);
        }
    };

    struct Transition {
        std::string toID;

        std::string startID;
        std::string endID;

        std::string signal;
    };

    std::string startStateID;
    std::vector<State> states;
    std::vector<Transition> transitions;
};

struct MooreCell {
    std::string newNodeID;
};

struct MooreToMealyHeader {
    std::string startNodeID;
    std::string newNodeID;
    std::string output;

    auto operator<=>(const MooreToMealyHeader& other) const {
        return std::tie(startNodeID, output)
            <=> std::tie(other.startNodeID, other.output);
    }
};

struct MooreTable {
    std::vector<MooreToMealyHeader> headers;
    std::vector<std::string> columns;
    std::vector<MooreCell> cells;
};

struct AutomatonMealy {
    struct Transitions {
        std::string startID;
        std::string endID;

        std::string signal;
        std::string output;
    };

    std::string startStateID;
    std::vector<Transitions> transitions;
};

struct MealyCell {
    std::string endID;
    std::string output;
};

struct MealyTable {
    std::vector<std::string> headers;
    std::vector<std::string> columns;
    std::vector<MealyCell> cells;
};