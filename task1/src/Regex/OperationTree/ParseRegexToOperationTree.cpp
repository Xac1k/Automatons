#include "ParseRegexToOperationTree.h"

#include <stdexcept>

std::shared_ptr<regex::OperationNode> regex::ParseRegexToOperationTree::operator()(const std::string& regex) {
    auto group = std::static_pointer_cast<OperationNode>(std::make_shared<GroupNode>());
    group->slot = &group;
    m_activeNode = group;

    size_t symbolIndex = 0;
    for (const char symbol : regex) {
        if (symbol == '\\') {
            m_forceTerminal = true;
        } else if ((symbol == '*' || symbol == '+') && !m_forceTerminal) {
            HandleRepeatSymbol(symbol);
        } else if (symbol == '|' && !m_forceTerminal) {
            HandleOrSymbol(symbol);
        } else if (symbol == '(' && !m_forceTerminal) {
            HandleGroupIn(symbol);
        } else if (symbol == ')' && !m_forceTerminal) {
            HandleGroupOut();
        } else {
            HandleTerminal(symbol);
            m_forceTerminal = false;
        }
        if (m_verbose) {
            Printable::PrintSentenceWithCursor(regex, symbolIndex);
            Printable::PrintTree(group, GetActive());
            symbolIndex++;
        }
    }

    return group;
}

size_t regex::ParseRegexToOperationTree::DepthOf(std::shared_ptr<OperationNode> node) {
    size_t depth = 0;
    while (node->parent != nullptr) { depth++; node = node->parent; }

    return depth;
}

void regex::ParseRegexToOperationTree::HandleRepeatSymbol(const char symbol) {
    auto active = GetActive();

    if (active->Type() == OperationNode::Type::Terminal)
        throw std::invalid_argument("Can't proceed repeat operation under terminal");

    const auto repeat = std::make_shared<RepeatNode>();
    repeat->canOmit = symbol == '*';

    if (active->Type() == OperationNode::Type::Group) {
        const auto parentGroup = std::static_pointer_cast<GroupNode>(active);
        const auto child = parentGroup->children.back();
        parentGroup->children.pop_back();

        m_activeNode = repeat;
        InsertInActive(child);
        m_activeNode = active;
        InsertInActive(repeat);
    } else if (active->Type() == OperationNode::Type::Or) {
        const auto parentOr = std::static_pointer_cast<OrNode>(active);
        const auto child = parentOr->operands.back();
        parentOr->operands.pop_back();

        m_activeNode = repeat;
        InsertInActive(child);
        m_activeNode = active;
        InsertInActive(repeat);
    } else if (active->Type() == OperationNode::Type::Repeat) {
        const auto parentRepeat = std::static_pointer_cast<RepeatNode>(active);
        repeat->child = parentRepeat->child;
        parentRepeat->child.reset();
        InsertInActive(repeat);
    }
}

void regex::ParseRegexToOperationTree::HandleTerminal(const char symbol) {
    if (!CanIncorporateNode())
        throw std::invalid_argument("Incorrect operator near: " + std::string{symbol});
    auto terminal = std::make_unique<TerminalNode>(symbol);
    InsertInActive(std::move(terminal));
}

void regex::ParseRegexToOperationTree::HandleOrSymbol(const char symbol) {
    const auto active = GetActive();

    if (active->Type() == OperationNode::Type::Or) {
        const auto group = std::make_shared<GroupNode>();
        auto terminal = std::make_unique<TerminalNode>(symbol);
        group->children.push_back(std::move(terminal));

        const auto activeNode = std::static_pointer_cast<OrNode>(active);
        activeNode->operands.push_back(group);
        m_activeNode = group;
        return;
    }

    if (
        active->parent &&
        active->parent->Type() == OperationNode::Type::Or &&
        active->Type() == OperationNode::Type::Group
    ) {
        m_activeNode = active->parent;
        return;
    }

    if (active->Type() != OperationNode::Type::Group)
        throw std::invalid_argument(
            "Attempt to create or operation without left operands near: " +
            std::string{symbol});

    const auto groupNode = std::static_pointer_cast<GroupNode>(active);
    if (groupNode->children.empty())
        throw std::invalid_argument(
            "Attempt to create or operation without left operands near: " +
            std::string{symbol});

    const auto orNode = std::make_shared<OrNode>();
    ReplaceNode(groupNode, orNode);
    m_activeNode = orNode;

    InsertInActive(groupNode);
}

void regex::ParseRegexToOperationTree::ReplaceNode(const std::shared_ptr<OperationNode>& target, const std::shared_ptr<OperationNode>& replacement) {
    replacement->parent = target->parent;
    replacement->slot = target->slot;
    *target->slot = replacement;
}

void regex::ParseRegexToOperationTree::HandleGroupIn(const char symbol) {
    const auto group = std::make_shared<GroupNode>();

    InsertInActive(group);
    m_nestingLevelGroupEntry.push(DepthOf(group) - 1);
    m_activeNode = group;
}

std::shared_ptr<regex::OperationNode> regex::ParseRegexToOperationTree::GetActive() const {
    auto active = m_activeNode.lock();
    if (!active)
        throw std::invalid_argument("Attempt to access empty active node");
    return active;
}

bool regex::ParseRegexToOperationTree::CanIncorporateNode() const {
    const auto active = GetActive();
    return active->Type() == OperationNode::Type::Group ||
        active->Type() == OperationNode::Type::Or ||
        active->Type() == OperationNode::Type::Repeat;
}

void regex::ParseRegexToOperationTree::InsertInActive(const std::shared_ptr<OperationNode>& incorporated) {
    const auto active = GetActive();
    incorporated->parent = active;

    if (active->Type() == OperationNode::Type::Group) {
        const auto groupNode = std::static_pointer_cast<GroupNode>(active);
        groupNode->children.push_back(incorporated);
        incorporated->slot = &groupNode->children.back();
    } else if (active->Type() == OperationNode::Type::Or) {
        const auto orNode = std::static_pointer_cast<OrNode>(active);

        orNode->operands.push_back(incorporated);
        incorporated->parent = orNode;
        incorporated->slot = &orNode->operands.back();

        if (incorporated->Type() != OperationNode::Type::Repeat) {
            const auto group = std::make_shared<GroupNode>();
            m_activeNode = group;

            orNode->operands.push_back(group);
            group->parent = orNode;
            group->slot = &orNode->operands.back();
        }
    } else if (active->Type() == OperationNode::Type::Repeat) {
        const auto repeatNode = std::static_pointer_cast<RepeatNode>(active);
        if (repeatNode->child)
            throw std::invalid_argument("Attempt to erase child of repeat");

        repeatNode->child = incorporated;
        incorporated->slot = &repeatNode->child;
    }
}

void regex::ParseRegexToOperationTree::HandleGroupOut() {
    if (m_nestingLevelGroupEntry.empty())
        throw std::invalid_argument("Unclosed bracket");

    auto targetDepth = m_nestingLevelGroupEntry.top();
    m_nestingLevelGroupEntry.pop();

    auto active = GetActive();
    auto currentDepth = DepthOf(active);
    while (currentDepth != targetDepth) { currentDepth--; active = active->parent; }

    m_activeNode = active;
}
