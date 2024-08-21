#include <fstream>

#include "table.hpp"

int main() {
    constexpr auto file_mode{std::ios::binary | std::ios::out | std::ios::in};
    const std::string file_name{"database"};

<<<<<<< Updated upstream
    table.exec({StatementType::INSERT, {1, {"hello"}, {"world"}}});
=======
    std::fstream file{file_name, file_mode};

    if (!file.is_open()) {
        std::ofstream file_new{file_name};
        file_new.close();

        file.open(file_name, file_mode);
    }

    Table table{file};

    table.exec({StatementType::INSERT, {420, {"username"}, {"email"}}});
>>>>>>> Stashed changes
    table.exec({StatementType::SELECT});

    return 0;
}
