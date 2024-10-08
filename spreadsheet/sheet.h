#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <unordered_map>

struct CellHasher {
public:
    size_t operator()(const Position p) const {
        return i_hasher_(p.row) * seed + i_hasher_(p.col) * seed * seed;
    }

private:
    std::hash<int> i_hasher_;
    size_t seed = 17;
};

struct CellComparator {
public:
    bool operator()(const Position& lhs, const Position& rhs) const {
        return lhs == rhs;
    }
};

class Sheet : public SheetInterface {
public:
    using Table = std::unordered_map<Position, std::unique_ptr<Cell>, CellHasher, CellComparator>;

    ~Sheet() = default;

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    const Cell* GetCellPtr(Position pos) const;
    Cell* GetCellPtr(Position pos);

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
	Table cells_;
};