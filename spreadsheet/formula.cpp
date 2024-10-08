#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#ARITHM!";
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression)
    try : 
        ast_(ParseFormulaAST(expression))
    {}
    catch (...) {
        throw FormulaException("Invalid formula!");
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        const CellGetter cell_getter = [&sheet](const Position p)->double {
            if (!p.IsValid()) {
                throw FormulaError(FormulaError::Category::Ref);
            }

            const CellInterface* cell = sheet.GetCell(p);
            if (!cell) {
                return 0;
            }

            auto cell_value = cell->GetValue();

            if (std::holds_alternative<double>(cell_value)) {
                return std::get<double>(cell_value);
            }

            if (std::holds_alternative<std::string>(cell_value)) {
                std::string value = std::get<std::string>(cell_value);
                double result = 0;

                if (!value.empty()) {
                    std::istringstream in(value);
                    if (!(in >> result && in.eof())) {
                        throw FormulaError(FormulaError::Category::Value);
                    }
                }

                return result;
            }

            throw FormulaError(std::get<FormulaError>(cell->GetValue()));
        };

        try {
            return ast_.Execute(cell_getter);
        }
        catch(FormulaError& e) {
           return std::move(e);
        }   
    }

    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> cells;

        for (auto cell : ast_.GetCells()) {
            if (cell.IsValid()) {
                cells.push_back(cell);
            }
        }
        cells.resize(std::unique(cells.begin(), cells.end()) - cells.begin());
        return cells;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch(const std::exception& e) {
        throw FormulaException(e.what());
    }
}