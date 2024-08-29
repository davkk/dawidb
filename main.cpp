#include <format>
#include <fstream>

#include "table.hpp"
#include "tree.hpp"

int main() {
    std::fstream file{
        "database",
        std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc
    };

    Table table{file};

    for (uint32_t idx{1}; idx <= 5; idx++) {
        std::string username = std::format("hello{}", idx);
        std::string email = std::format("world{}", idx);

        Row row{};
        std::copy(username.begin(), username.end(), row.username.begin());
        std::copy(email.begin(), email.end(), row.email.begin());

        table.exec({StatementType::INSERT, {idx, row.username, row.email}});
    }

    table.exec({StatementType::SELECT});

    table.show();

    return 0;
}
