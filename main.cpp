#include <cassert>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <optional>
#include <span>

constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t MAX_PAGES = 100;

constexpr uint32_t SIZE_ID = sizeof(uint32_t);
constexpr uint32_t SIZE_USERNAME = 32;
constexpr uint32_t SIZE_EMAIL = 255;

struct Row {
    uint32_t id{};
    std::array<char, SIZE_USERNAME> username{};
    std::array<char, SIZE_EMAIL> email{};
};

constexpr uint32_t ROW_SIZE = SIZE_ID + SIZE_USERNAME + SIZE_EMAIL;
constexpr uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr uint32_t MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;

enum class StatementType {
    INSERT,
    SELECT,
};

struct Statement {
    StatementType type{};
    Row row_to_insert{};
};

struct Table {
    size_t num_rows{};
    std::array<std::optional<std::array<std::byte, PAGE_SIZE>>, MAX_PAGES>
        pages{};
};

// span == view into memory, doesn't own it
// byte[] == actual memory

std::optional<std::span<std::byte>> row_slot(Table& table, size_t row_num) {
    auto page_num{row_num / ROWS_PER_PAGE};

    if (page_num >= MAX_PAGES) {
        return std::nullopt;
    }
    assert(page_num < MAX_PAGES);

    auto& page = table.pages[page_num];
    if (!page) {
        page = std::array<std::byte, PAGE_SIZE>{};
    }

    auto offset = (row_num % ROWS_PER_PAGE) * ROW_SIZE;
    return std::span{page->data() + offset, ROW_SIZE};
}

void serialize_row(const Row& src, std::span<std::byte> dest) {
    std::memcpy(dest.data(), &src.id, SIZE_ID);

    auto username_span = dest.subspan(SIZE_ID, SIZE_USERNAME);
    std::memcpy(username_span.data(), src.username.begin(), SIZE_USERNAME);

    auto email_span = dest.subspan(SIZE_ID + SIZE_USERNAME, SIZE_EMAIL);
    std::memcpy(email_span.data(), src.email.begin(), SIZE_EMAIL);
}

void deserialize_row(std::span<std::byte> src, Row& dest) {
    std::memcpy(&dest.id, src.data(), SIZE_ID);

    auto username_span = src.subspan(SIZE_ID, SIZE_USERNAME);
    std::memcpy(dest.username.data(), username_span.data(), SIZE_USERNAME);

    auto email_span = src.subspan(SIZE_ID + SIZE_USERNAME, SIZE_EMAIL);
    std::memcpy(dest.email.data(), email_span.data(), SIZE_EMAIL);
}

void exec(const Statement& statement, Table& table) {
    switch (statement.type) {
    case StatementType::SELECT: {
        std::cout << "\nselect statement\n";

        Row row{};
        for (size_t idx{0}; idx < table.num_rows; idx++) {
            const auto slot_opt{row_slot(table, idx)};
            if (auto slot{*slot_opt}; slot_opt) {
                deserialize_row(slot, row);
                std::cout << std::format(
                    "{}|{}|{}\n", row.id, row.username.data(), row.email.data()
                );
            }
        }

        break;
    }
    case StatementType::INSERT: {
        std::cout << "\ninsert statement\n";

        if (table.num_rows == MAX_ROWS) {
            return;
        }

        const auto slot_opt{row_slot(table, table.num_rows)};
        if (auto slot{*slot_opt}; slot_opt) {
            const auto& row{statement.row_to_insert};
            std::cout << std::format(
                "{}|{}|{}\n", row.id, row.username.data(), row.email.data()
            );
            serialize_row(row, slot);
            table.num_rows++;
        }
        break;
    }
    }
}

int main() {
    Table table{};

    exec({StatementType::INSERT, {1, {"hello"}, {"world"}}}, table);
    exec({StatementType::INSERT, {2, {"jane"}, {"doe"}}}, table);

    exec({StatementType::SELECT}, table);

    return 0;
}
