#pragma once
#include "../../New/AutomatTables/AutomatTables.h"
#include "../../Old/Automatons.h"
#include "../OperationTree/ParseRegexToOperationTree.h"

namespace regex {
    template <typename targetType>
    class Transform {};

    template <>
    class Transform<AutomatonMoore> {
    public:
        AutomatonMoore operator()(const std::shared_ptr<OperationNode>& operationRoot);

    private:
        AutomatonMoore m_moore;
        size_t m_stateCounter = 0;

        std::string GetNextStateID();
        void WorkBetween(const std::string& source, const std::shared_ptr<OperationNode>& operation, const std::string& target);
    };

    template <>
    class Transform<MooreTable> {
    public:
        MooreTable operator()(const std::shared_ptr<OperationNode>& operationRoot) const;
    };
}
