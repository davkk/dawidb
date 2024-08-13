#include "table.hpp"

int main() {
    Table table{};

    table.exec({StatementType::INSERT, {1, "hello", "world"}});
    table.exec({StatementType::INSERT, {2, "jane", "doe"}});

    table.exec({StatementType::SELECT});

    return 0;
}
