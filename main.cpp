#include <cassert>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <utility>

constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t MAX_PAGES = 100;

constexpr uint32_t SIZE_ID = sizeof(uint32_t);
constexpr uint32_t SIZE_USERNAME = 32;
constexpr uint32_t SIZE_EMAIL = 255;

constexpr uint32_t ROW_SIZE = SIZE_ID + SIZE_USERNAME + SIZE_EMAIL;
constexpr uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr uint32_t MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;

template <typename T, typename U>
using WithError = std::pair<T, std::optional<U>>;

struct Row {
    uint32_t id{};
    std::array<char, SIZE_USERNAME> username{};
    std::array<char, SIZE_EMAIL> email{};

    void serialize(std::span<char> dest) {
        assert(dest.size() == ROW_SIZE);

        std::memcpy(dest.data(), &this->id, SIZE_ID);

        auto username_span = dest.subspan(SIZE_ID, SIZE_USERNAME);
        std::memcpy(
            username_span.data(), this->username.data(), SIZE_USERNAME
        );

        auto email_span = dest.subspan(SIZE_ID + SIZE_USERNAME, SIZE_EMAIL);
        std::memcpy(email_span.data(), this->email.data(), SIZE_EMAIL);
    }

    void deserialize(std::span<char> src) {
        assert(src.size() == ROW_SIZE);

        std::memcpy(&this->id, src.data(), SIZE_ID);

        auto username_span = src.subspan(SIZE_ID, SIZE_USERNAME);
        std::memcpy(
            this->username.data(), username_span.data(), SIZE_USERNAME
        );

        auto email_span = src.subspan(SIZE_ID + SIZE_USERNAME, SIZE_EMAIL);
        std::memcpy(this->email.data(), email_span.data(), SIZE_EMAIL);
    }
};

enum class StatementType {
    INSERT,
    SELECT,
};

struct Statement {
    StatementType type{};
    Row row_to_insert{};
};

using Page = std::array<char, PAGE_SIZE>;

enum class PagerError {
    OUT_OF_RANGE,
};

class Table {
public:
    void exec(Statement&& statement) {
        switch (statement.type) {
        case StatementType::SELECT: {
            std::cout << "\nselect statement\n";

            Row row{};
            for (size_t idx{0}; idx < num_rows; idx++) {
                const auto [slot, err]{find_slot(idx)};
                if (not err) {
                    row.deserialize(slot);
                    std::cout << std::format(
                        "{}|{}|{}\n", row.id, row.username.data(),
                        row.email.data()
                    );
                }
            }

            break;
        }
        case StatementType::INSERT: {
            std::cout << "\ninsert statement\n";

            if (num_rows == MAX_ROWS) {
                return;
            }

            const auto [slot, err]{find_slot(num_rows)};
            if (not err) {
                auto& row{statement.row_to_insert};
                std::cout << std::format(
                    "{}|{}|{}\n", row.id, row.username.data(), row.email.data()
                );
                row.serialize(slot);
                num_rows++;
            }
            break;
        }
        }
    }

private:
    size_t num_rows{};
    std::array<std::optional<Page>, MAX_PAGES> pages{};

    auto find_slot(size_t row_num) -> WithError<std::span<char>, PagerError> {
        auto page_num{row_num / ROWS_PER_PAGE};

        if (page_num >= MAX_PAGES) {
            return {std::span<char>{}, PagerError::OUT_OF_RANGE};
        }
        assert(page_num < MAX_PAGES);

        auto& page = pages[page_num];
        if (!page) {
            page = Page{};
        }

        auto offset = (row_num % ROWS_PER_PAGE) * ROW_SIZE;
        auto page_span = std::span<char>{*page};
        return {page_span.subspan(offset, ROW_SIZE), std::nullopt};
    }
};

int main() {
    Table table{};

    table.exec({StatementType::INSERT, {1, "hello", "world"}});
    table.exec({StatementType::INSERT, {2, "jane", "doe"}});

    table.exec({StatementType::SELECT});

    return 0;
}
