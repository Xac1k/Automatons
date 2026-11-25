#pragma once
#include <cstdint>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace regex {

class OperationNode {
public:
    enum class Type : uint8_t { Repeat, Or, Group, Terminal };
    using TerminalType = char;

    virtual ~OperationNode() = default;
    virtual Type Type() = 0;
    [[nodiscard]] virtual std::string Label() const = 0;
    std::shared_ptr<OperationNode> parent = nullptr;
    std::shared_ptr<OperationNode>* slot = nullptr;
};

class RepeatNode : public OperationNode {
public:
    std::shared_ptr<OperationNode> child = nullptr;
    bool canOmit = false;
    [[nodiscard]] std::string Label() const override { return canOmit ? "Repeat[*]" : "Repeat[+]"; }
    enum Type Type() override { return Type::Repeat; }
};

class OrNode : public OperationNode {
public:
    std::vector<std::shared_ptr<OperationNode>> operands{};
    [[nodiscard]] std::string Label() const override { return "Or"; }
    enum Type Type() override { return Type::Or; }
};

class GroupNode : public OperationNode {
public:
    std::vector<std::shared_ptr<OperationNode>> children{};
    [[nodiscard]] std::string Label() const override { return "Group"; }
    enum Type Type() override { return Type::Group; };
};

class TerminalNode : public OperationNode {
public:
    explicit TerminalNode(const TerminalType state) : terminal{state} {};
    TerminalType terminal{};
    [[nodiscard]] std::string Label() const override { return std::string("'") + terminal + "'"; };
    enum Type Type() override { return Type::Terminal; };
};


class ParseRegexToOperationTree {
public:
    explicit ParseRegexToOperationTree(const bool verbose = false): m_verbose(verbose) {}

    std::shared_ptr<OperationNode> operator()(const std::string& regex);
    static size_t DepthOf(std::shared_ptr<OperationNode> node);

private:
    bool m_forceTerminal = false;
    std::weak_ptr<OperationNode> m_activeNode{};
    std::stack<size_t> m_nestingLevelGroupEntry{};
    bool m_verbose;

    void HandleRepeatSymbol(char symbol);
    void HandleTerminal(char symbol);
    void HandleOrSymbol(char symbol);
    static void ReplaceNode(const std::shared_ptr<OperationNode>& target, const std::shared_ptr<OperationNode>& replacement);
    void HandleGroupIn(char symbol);
    [[nodiscard]] std::shared_ptr<OperationNode> GetActive() const;
    [[nodiscard]] bool CanIncorporateNode() const;
    void InsertInActive(const std::shared_ptr<OperationNode>& incorporated);
    void HandleGroupOut();
};

namespace Printable {
    struct Edge {
        std::string label;                  // "" — без подписи
        std::shared_ptr<OperationNode> node;
    };

    inline std::vector<Edge> ChildrenOf(OperationNode& node) {
        std::vector<Edge> out;
        switch (node.Type()) {
        case OperationNode::Type::Group: {
            const auto& g = static_cast<const GroupNode&>(node);
            for (size_t i = 0; i < g.children.size(); ++i)
                out.push_back({std::to_string(i), g.children[i]});
            break;
        }
        case OperationNode::Type::Or: {
            const auto& o = static_cast<const OrNode&>(node);
            for (size_t i = 0; i < o.operands.size(); ++i)
                out.push_back({std::to_string(i), o.operands[i]});
            break;
        }
        case OperationNode::Type::Repeat: {
            const auto& r = static_cast<const RepeatNode&>(node);
            if (r.child)
                out.push_back({"", r.child});
            break;
        }
        case OperationNode::Type::Terminal:
            break;
        }
        return out;
    }

    inline void PrintRec(const std::shared_ptr<OperationNode>& node,
                     const std::string& prefix,
                     bool isLast,
                     const std::shared_ptr<OperationNode>& active,
                     const char* yellow,
                     const char* reset)
    {
        const bool isActive = (node && node == active);

        std::cout << prefix << (isLast ? "└── " : "├── ");
        if (isActive) std::cout << yellow;
        std::cout << (node ? node->Label() : "<null>");
        if (isActive) std::cout << reset;
        std::cout << '\n';

        if (!node) return;

        const auto kids = ChildrenOf(*node);
        const std::string nextPrefix = prefix + (isLast ? "    " : "│   ");
        for (size_t i = 0; i < kids.size(); ++i)
            PrintRec(kids[i].node, nextPrefix, i + 1 == kids.size(),
                     active, yellow, reset);
    }

    inline void PrintTree(const std::shared_ptr<OperationNode>& root,
                      const std::shared_ptr<OperationNode>& active) {
        if (!root) { std::cout << "<empty>\n"; return; }

        // ANSI: жёлтый текст, сброс
        constexpr const char* kYellow = "\033[33m";
        constexpr const char* kReset  = "\033[0m";

        auto colorStart = [&](const std::shared_ptr<OperationNode>& n) -> const char* {
            return (n && n == active) ? kYellow : "";
        };
        auto colorEnd = [&](const std::shared_ptr<OperationNode>& n) -> const char* {
            return (n && n == active) ? kReset : "";
        };

        std::cout << colorStart(root) << root->Label() << colorEnd(root) << '\n';

        const auto kids = ChildrenOf(*root);
        for (size_t i = 0; i < kids.size(); ++i) {
            PrintRec(kids[i].node, "", i + 1 == kids.size(),
                     active, kYellow, kReset);
        }
    }

    inline void PrintSentenceWithCursor(const std::string& sentence, const size_t cursor) {
        constexpr auto kViolent = "\033[35m";
        constexpr auto kReset  = "\033[0m";

        auto colorStart = [](const bool need) -> const char* {
            return (need) ? kViolent : "";
        };
        auto colorEnd = [](const bool need) -> const char* {
            return (need) ? kReset : "";
        };

        for (size_t i = 0; i < sentence.size(); ++i) {
            std::cout << colorStart(cursor == i) << sentence[i] << colorEnd(cursor  == i);
        }
        std::cout << '\n';;
    }
}

}