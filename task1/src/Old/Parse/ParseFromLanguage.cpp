#include "ParseFromLanguage.h"

#include <string>
#include <set>
#include <sstream>
#include <regex>

// ┌── side: left ────────────────────────────────────────────────────────────┐
// │ language:                                                                │
// │   S -> S.a | S.b | C.c                                                   │
// │   A -> B.a | S.b | OUTPUT.c                                              │
// │   B -> B.b | B.a | S.c | OUTPUT.b                                        │
// │   C -> A.a | B.b | S.c | OUTPUT.a                                        │
// └──────────────────────────────────────────────────────────────────────────┘
//
// 1. куда
// 2. откуда
// 3. по какому символу
// Проход от OUTPUT

// ┌── side: right ───────────────────────────────────────────────────────────┐
// │ language:                                                                │
// │   S -> a.S | b.S | c.C                                                   │
// │   A -> a.B | b.S | c.OUTPUT                                              │
// │   B -> b.B | a.B | c.S | b.OUTPUT                                        │
// │   C -> a.A | b.B | c.S | a.OUTPUT                                        │
// └──────────────────────────────────────────────────────────────────────────┘
//
// 1. откуда
// 2. по какому символу
// 3. куда
// Проход от первой строки

AutomatonMoore ParseFromLanguage(const Side side, const std::string& language) {
    auto isLeft = side == Side::left;

    AutomatonMoore automaton;
    std::set<std::string> states;

    bool isFirst = true;
    std::string axiomRootFirstLine;
    std::string line;
    std::istringstream ss(language);
    while (std::getline(ss, line)) {
        std::istringstream lineSS(line);
        std::string axiomRoot, axiom;

        lineSS >> axiomRoot;
        if (isFirst) {
            axiomRootFirstLine = axiomRoot;
            automaton.startStateID = isLeft ? "OUTPUT" : axiomRoot;
            isFirst = false;
        }
        while (lineSS >> axiom) {
            std::regex axiomRegex(R"(\s*([\wА-Яа-я]+)\.([\wА-Яа-я]+)\s*)", std::regex::ECMAScript);
            std::smatch match;
            if (std::regex_search(axiom, match, axiomRegex); !match.empty()) {
                auto axiomConsequence = isLeft ? match[1].str() : match[2].str();
                auto signal = isLeft ? match[2].str() : match[1].str();

                states.insert(axiomRoot);
                states.insert(axiomConsequence);

                automaton.transitions.push_back({
                    .startID = isLeft ? axiomConsequence : axiomRoot,
                    .endID = isLeft ? axiomRoot : axiomConsequence,
                    .signal = signal
                });
            }
        }
    }

    for (const auto& stateName : states) {
        AutomatonMoore::State state{
            .stateID = stateName,
            .output = !isLeft
                ? stateName == "OUTPUT" ? "OUTPUT" : ""
                : stateName == axiomRootFirstLine ? "OUTPUT" : "",
            .name = stateName
        };
        automaton.states.push_back(state);
    }

    return automaton;
}


