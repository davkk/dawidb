#pragma once

#include "row.hpp"

enum class StatementType {
    INSERT,
    SELECT,
};

struct Statement {
    StatementType type;
    Row row_to_insert;
};
