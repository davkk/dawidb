#include <format>
#include <fstream>

#include "table.hpp"

int main() {
    std::fstream file{"database", std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc};

    Table table{file};

    for (uint32_t idx{1}; idx <= 5; idx++) {
        Row row{.id = idx};

        std::string username = std::format("hello{}", idx);
        std::string email = std::format("world{}", idx);

        std::copy(username.begin(), username.end(), row.username);
        std::copy(email.begin(), email.end(), row.email);

        table.exec({StatementType::INSERT, row});
    }

    table.exec({StatementType::SELECT});

    table.show();

    return 0;
}
