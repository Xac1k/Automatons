#include "Parse.h"

#include <regex>
#include <sstream>

#include "../../StringUtils/StringUtils.h"

AutomatonMealy MealyParse(const std::string& text) {
    AutomatonMealy automaton;
    const std::regex mealyRegex(
        R"(type:\s*(\w+)\s*start:\s*(\w+)\s*transitions:\s*((\w+\s*\w+\s*[\w\d_]+\s*\/\s*[\w\d]*\s*)*))",
        std::regex::ECMAScript
    );
    const std::regex mealyTransitionRegex(
        R"(\s*(\w+)\s*(\w+)\s*([\w\d_]+)\s*\/\s*([\w\d]*)\s*)",
        std::regex::ECMAScript
    );

    std::smatch automatonMatch;
    if (std::regex_search(text, automatonMatch, mealyRegex)) {
        automaton.startStateID = automatonMatch[2].str();
        auto transitions = automatonMatch[3].str();
        Trim(transitions);

        std::string lineTransition;
        std::istringstream transitionsInput(transitions);
        while (std::getline(transitionsInput, lineTransition)) {
            std::smatch transitionMatch;
            std::regex_search(lineTransition, transitionMatch, mealyTransitionRegex);

            AutomatonMealy::Transitions transition;
            transition.startID = transitionMatch[1];
            transition.endID = transitionMatch[2];
            transition.signal = transitionMatch[3];
            transition.output = transitionMatch[4];

            automaton.transitions.push_back(transition);
        }
    } else {
        throw std::runtime_error("Couldn't find mealy matching.");
    }

    return automaton;
};

AutomatonMoore MooreParse(const std::string& text) {
    AutomatonMoore automaton;
    const std::regex mooreRegex(
        R"(type:\s*(\w+)\s*start:\s*(\w+)\s*states:(\s*(\w+\s*\|\s*[\wа-яА-Я_0-9]+\s*\/\s*[а-яА-Я_\w]*\s*)*)transitions:\s*((\w+\s*\w+\s*[\wа-яА-ЯёЁ\d_]+\s*)*))",
        std::regex::ECMAScript
    );
    const std::regex mooreStateRegex(
        R"(\s*(\w+)\s*\|\s*([\w_\d]+)\s*\/\s*([\d_\w]*)\s*)",
        std::regex::ECMAScript
    );
    const std::regex mooreTransitionRegex(
        R"((\w+)\s*(\w+)\s*([\wа-яА-ЯёЁ\d_]+))",
        std::regex::ECMAScript
    );

    std::smatch automatonMatch;
    if (std::regex_search(text, automatonMatch, mooreRegex)) {
        automaton.startStateID = automatonMatch[2].str();
        auto states = automatonMatch[3].str();
        Trim(states);
        auto transitions = automatonMatch[5].str();
        Trim(transitions);

        std::string lineState;
        std::istringstream stateInput(states);
        while (std::getline(stateInput, lineState, '\n')) {
            std::smatch stateMatch;
            std::regex_search(lineState, stateMatch, mooreStateRegex);

            AutomatonMoore::State state;
            state.name = stateMatch[1];
            state.stateID = stateMatch[2];
            state.output = stateMatch[3];

            automaton.states.push_back(state);
        }

        std::string lineTransition;
        std::istringstream transitionsInput(transitions);
        while (std::getline(transitionsInput, lineTransition)) {
            std::smatch transitionMatch;
            std::regex_search(lineTransition, transitionMatch, mooreTransitionRegex);

            AutomatonMoore::Transition transition;
            transition.startID = transitionMatch[1];
            transition.endID = transitionMatch[2];
            transition.signal = transitionMatch[3];

            automaton.transitions.push_back(transition);
        }
    } else {
        throw std::runtime_error("Couldn't find moore matching.");
    }

    return automaton;
};