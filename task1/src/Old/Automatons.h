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