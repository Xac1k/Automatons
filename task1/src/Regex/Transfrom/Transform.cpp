#include "Transform.h"
#include "../../New/From/From.h"

constexpr auto EMPTY = "EMPTY";
constexpr auto NO_OUTPUT = "";

MooreTable regex::Transform<MooreTable>::operator()(const std::shared_ptr<OperationNode>& operationRoot) const {
    Transform<AutomatonMoore> transform;
    return From(transform(operationRoot));
}

AutomatonMoore regex::Transform<AutomatonMoore>::operator()(const std::shared_ptr<OperationNode>& operationRoot) {
    m_moore = AutomatonMoore{};

    const auto startID = GetNextStateID();
    const auto endID = GetNextStateID();

    m_moore.startStateID = startID;
    m_moore.states.push_back(AutomatonMoore::State{
        .stateID = startID,
        .output = "",
        .name = startID
    });

    m_moore.states.push_back(AutomatonMoore::State{
        .stateID = endID,
        .output = "OUTPUT",
        .name = endID
    });

    WorkBetween(startID, operationRoot, endID);
    return m_moore;
}

std::string regex::Transform<AutomatonMoore>::GetNextStateID() {
    return "q" + std::to_string(m_stateCounter++);
}

void regex::Transform<AutomatonMoore>::WorkBetween(
    const std::string& source,
    const std::shared_ptr<OperationNode>& operation,
    const std::string& target
) {
    const auto operationType = operation->Type();
    switch (operationType) {
    case OperationNode::Type::Or: {
        auto orNode = std::static_pointer_cast<OrNode>(operation);
        for (auto& operand : orNode->operands) {
            auto newStateID = GetNextStateID();
            const AutomatonMoore::State newState{
                .stateID = newStateID,
                .output = NO_OUTPUT,
                .name = newStateID
            };

            const AutomatonMoore::Transition sourceToNewState{
                .startID = source,
                .endID = newStateID,
                .signal = EMPTY
            };

            m_moore.states.push_back(newState);
            m_moore.transitions.push_back(sourceToNewState);
            WorkBetween(newStateID, operand, target);
        }
    } break;
    case OperationNode::Type::Group: {
        auto group = std::static_pointer_cast<GroupNode>(operation);
        auto from = source;
        for (size_t i = 0; i < group->children.size(); i++) {
            auto isLast = i == (group->children.size() - 1);
            auto nextOperation = group->children.at(i);

            if (!isLast) {
                auto newStateID = GetNextStateID();

                const AutomatonMoore::State newState{
                    .stateID = newStateID,
                    .output = NO_OUTPUT,
                    .name = newStateID
                };
                m_moore.states.push_back(newState);
                WorkBetween(from, nextOperation, newStateID);
                from = newStateID;
            } else {
                WorkBetween(from, nextOperation, target);
            }
        }
    } break;
    case OperationNode::Type::Repeat: {
        const auto repeat = std::static_pointer_cast<RepeatNode>(operation);
        const auto nextOperation = repeat->child;
        if (repeat->canOmit) { // *
            auto newStateID = GetNextStateID();
            const AutomatonMoore::Transition sourceToTarget{
                .startID = source,
                .endID = target,
                .signal = EMPTY
            };
            const AutomatonMoore::Transition newNodeToSource{
                .startID = newStateID,
                .endID = source,
                .signal = EMPTY
            };
            const AutomatonMoore::Transition newNodeToTarget{
                .startID = newStateID,
                .endID = target,
                .signal = EMPTY
            };
            const AutomatonMoore::State newState{
                .stateID = newStateID,
                .output = NO_OUTPUT,
                .name = newStateID
            };
            m_moore.transitions.push_back(sourceToTarget);
            m_moore.transitions.push_back(newNodeToSource);
            m_moore.transitions.push_back(newNodeToTarget);

            m_moore.states.push_back(newState);
            WorkBetween(source, nextOperation, newStateID);
        } else { // +
            const AutomatonMoore::Transition transition{
                .startID = target,
                .endID = source,
                .signal = EMPTY
            };
            m_moore.transitions.push_back(transition);
            WorkBetween(source, nextOperation, target);
        }
    } break;
    case OperationNode::Type::Terminal: {
        const auto terminal = std::static_pointer_cast<TerminalNode>(operation);
        const auto symbol = std::string{terminal->terminal};

        const AutomatonMoore::Transition transition{
            .startID = source,
            .endID = target,
            .signal = symbol,
        };

        m_moore.transitions.push_back(transition);
    } break;
    }
}


