#include <format>
#include <fstream>

#include "table.hpp"

int main() {
    constexpr auto file_mode{std::ios::binary | std::ios::out | std::ios::in};
    const std::string file_name{"database"};

    std::fstream file{file_name, file_mode};

    if (!file.is_open()) {
        std::ofstream file_new{file_name};
        file_new.close();

        file.open(file_name, file_mode);
    }

    Table table{file};

    for (uint32_t idx{1}; idx <= 4; idx++) {
        std::string username = std::format("hello{}", idx);
        std::string email = std::format("world{}", idx);

        Row row{};
        std::copy(username.begin(), username.end(), row.username.begin());
        std::copy(email.begin(), email.end(), row.email.begin());

        table.exec({StatementType::INSERT, {idx, row.username, row.email}});
    }

    table.exec({StatementType::SELECT});

    return 0;
}
