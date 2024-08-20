#include "table.hpp"

int main() {
    Table table{"database"};

    table.exec({StatementType::INSERT, {1, {"hello"}, {"world"}}});
    table.exec({StatementType::SELECT});

    return 0;
}
