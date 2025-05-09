#pragma once

#include "formula.h"
#include "common.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;
    
private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    bool HasCircularDependency(const Impl& impl) const;

    void UpdateDependencies();
    void InvalidateCellsCache(bool flag = false);

    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;
    std::unordered_set<Cell*> dependent_cells_;
    std::unordered_set<Cell*> referenced_cells_;
};