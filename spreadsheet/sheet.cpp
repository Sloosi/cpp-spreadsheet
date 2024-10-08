#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

void Sheet::SetCell(Position pos, std::string text) {
    if(!pos.IsValid()) {
        throw InvalidPositionException("Invalid position!");
    }
    
    const auto& cell = cells_.find(pos);
    if(cell == cells_.end()) {
        cells_.emplace(pos, std::make_unique<Cell>(*this));
    }

    cells_.at(pos)->Set(std::move(text));
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return GetCellPtr(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    return GetCellPtr(pos);
}

const Cell* Sheet::GetCellPtr(Position pos) const {
    if(!pos.IsValid()) {
        throw InvalidPositionException("Invalid position!");
    }

    const auto cell = cells_.find(pos);
    if (cell == cells_.end()) {
        return nullptr;
    }

    return cells_.at(pos).get();
}

Cell* Sheet::GetCellPtr(Position pos) {
    return const_cast<Cell*>(const_cast<const Sheet*>(this)->GetCellPtr(pos));
}

void Sheet::ClearCell(Position pos) {
    if(!pos.IsValid()) {
        throw InvalidPositionException("Invalid position!");
    }

    const auto& cell = cells_.find(pos);
    if (cell != cells_.end() && cell->second != nullptr) {
        cell->second->Clear();

        if (!cell->second->IsReferenced()) {
            cell->second.reset();
        }
    }
}

Size Sheet::GetPrintableSize() const {
    Size result;

    for (auto it = cells_.begin(); it != cells_.end(); ++it) {
        if (it->second != nullptr) {
            const int col = it->first.col;
            const int row = it->first.row;
            result.rows = std::max(result.rows, row + 1);
            result.cols = std::max(result.cols, col + 1);
        }
    }

    return result;
}

void Sheet::PrintValues(std::ostream& output) const {
    auto [printable_rows, printable_cols] = GetPrintableSize();

    for (int row = 0; row < printable_rows; ++row) {
        for (int col = 0; col < printable_cols; ++col) {
            if (col > 0) {
                output << "\t";
            }

            const auto& cell = cells_.find({ row, col });
            if (cell != cells_.end() && cell->second != nullptr && !cell->second->GetText().empty()) {
                std::visit([&](const auto value) { output << value; }, cell->second->GetValue());
            }
        }
        output << "\n";
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    auto [printable_rows, printable_cols] = GetPrintableSize();

    for (int row = 0; row < printable_rows; ++row) {
        for (int col = 0; col < printable_cols; ++col) {
            if (col > 0) {
                output << "\t";
            }

            const auto& cell = cells_.find({ row, col });
            if (cell != cells_.end() && cell->second != nullptr && !cell->second->GetText().empty()) {
                output << cell->second->GetText();
            }
        }
        output << "\n";
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}