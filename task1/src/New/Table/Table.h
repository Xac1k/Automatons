#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <ranges>
#include <stdexcept>

template<typename CellPayload>
class TableColumn {
public:
    using RowID = std::string;

    CellPayload& GetRowByID(const RowID& id) {
        auto it = rows.find(id);
        if (it == rows.end()) {
            throw std::invalid_argument("Invalid row id");
        }
        return it->second;
    }

    TableColumn& AddNewRow(const RowID& newRowId, const CellPayload& newCell) {
        auto it = rows.find(newRowId);
        if (it != rows.end()) {
            throw std::invalid_argument("Row with that ID already exists");
        }

        rows[newRowId] = newCell;
        return *this;
    }

    [[nodiscard]] std::vector<RowID> GetRowIDs() const {
        std::vector<RowID> rowIDs;
        rowIDs.reserve(rows.size());
        for (const auto& [rowID, _] : rows) {
            rowIDs.push_back(rowID);
        }
        std::ranges::sort(rowIDs, [](const std::string& first, const std::string& second) {
            if (first.length() != second.length()) {
                return first.length() < second.length();
            }
            return first < second;
        });
        return rowIDs;
    }

private:
    std::unordered_map<RowID, CellPayload> rows;
};


template<typename ColumnPayload, typename CellPayload>
class Table {
public:
    using ColumnID = std::string;

    struct TableColumnData {
        TableColumn<CellPayload> column;
        ColumnPayload payload;
    };

    TableColumnData& GetColumnByID(const ColumnID& id) {
        auto it = columns.find(id);
        if (it == columns.end()) {
            throw std::invalid_argument("Invalid column id");
        }
        return it->second;
    }

    Table& AddNewColumn(const ColumnID& newColumnId, TableColumnData& newColumn) {
        auto it = columns.find(newColumnId);
        if (it != columns.end()) {
            throw std::invalid_argument("Column with that ID already exists");
        }

        columns[newColumnId] = newColumn;
        return *this;
    }

    [[nodiscard]] std::vector<ColumnID> GetColumnIDs() const {
        std::vector<ColumnID> columnIDs;
        columnIDs.reserve(columns.size());
        for (const auto& [columnID, _] : columns) {
            columnIDs.push_back(columnID);
        }
        std::ranges::sort(columnIDs, [](const std::string& first, const std::string& second) {
            if (first.length() != second.length()) {
                return first.length() < second.length();
            }
            return first < second;
        });
        return columnIDs;
    }

private:
    std::map<ColumnID, TableColumnData> columns;
};
