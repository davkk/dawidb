#include <cassert>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <span>

constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t MAX_PAGES = 100;

constexpr uint32_t SIZE_ID = sizeof(uint32_t);
constexpr uint32_t OFFSET_ID = 0;
constexpr uint32_t SIZE_USERNAME = 32;
constexpr uint32_t OFFSET_USERNAME = SIZE_ID;
constexpr uint32_t SIZE_EMAIL = 255;
constexpr uint32_t OFFSET_EMAIL = SIZE_ID + SIZE_USERNAME;

constexpr uint32_t ROW_SIZE = SIZE_ID + SIZE_USERNAME + SIZE_EMAIL;
constexpr uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr uint32_t MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;

template <typename Type, typename Error>
using WithError = std::pair<Type, std::optional<Error>>;

struct Row {
    uint32_t id;
    std::array<char, SIZE_USERNAME> username;
    std::array<char, SIZE_EMAIL> email;

    void serialize_into(const std::span<char> dest) {
        assert(dest.size() == ROW_SIZE);
        std::span<char> span;

        span = dest.subspan(OFFSET_ID, SIZE_ID);
        assert(span.size() == SIZE_ID);
        std::memcpy(span.data(), &id, SIZE_ID);

        span = dest.subspan(OFFSET_USERNAME, SIZE_USERNAME);
        assert(span.size() == SIZE_USERNAME);
        std::copy(username.begin(), username.end(), span.begin());

        span = dest.subspan(OFFSET_EMAIL, SIZE_EMAIL);
        assert(span.size() == SIZE_EMAIL);
        std::copy(email.begin(), email.end(), span.begin());
    }

    void deserialize_from(const std::span<char> src) {
        assert(src.size() == ROW_SIZE);
        std::span<char> span;

        span = src.subspan(OFFSET_ID, SIZE_ID);
        assert(span.size() == SIZE_ID);
        std::memcpy(&id, span.data(), SIZE_ID);

        span = src.subspan(OFFSET_USERNAME, SIZE_USERNAME);
        assert(span.size() == SIZE_USERNAME);
        std::copy(span.begin(), span.end(), username.begin());

        span = src.subspan(OFFSET_EMAIL, SIZE_EMAIL);
        assert(span.size() == SIZE_EMAIL);
        std::copy(span.begin(), span.end(), email.begin());
    }
};

enum class StatementType {
    INSERT,
    SELECT,
};

struct Statement {
    StatementType type;
    Row row_to_insert;
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
            std::cout << pages[0].data() << std::endl;
            std::cout << "\nselect statement\n";

            Row row{};

            for (size_t idx{0}; idx < num_rows; idx++) {
                const auto [slot, err]{find_slot(idx)};
                if (not err) {
                    row.deserialize_from(slot);
                    std::cout << std::format(
                        "{}|{}|{}\n", row.id, row.username.data(),
                        row.email.data()
                    );
                }
            }

            return;
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
                row.serialize_into(slot);
                num_rows++;
            }

            return;
        }
        }
    }

private:
    size_t num_rows{0};
    std::array<Page, MAX_PAGES> pages{};

    auto find_slot(size_t row_num) -> WithError<std::span<char>, PagerError> {
        auto page_num{row_num / ROWS_PER_PAGE};

        if (page_num >= MAX_PAGES) {
            return {{}, PagerError::OUT_OF_RANGE};
        }

        auto offset = (row_num % ROWS_PER_PAGE) * ROW_SIZE;

        assert(page_num < pages.size());
        assert(offset < pages[page_num].size());

        return {{&pages[page_num][offset], ROW_SIZE}, std::nullopt};
    }
};

int main() {
    Table table{};

    table.exec({StatementType::INSERT, {1, "hello", "world"}});
    table.exec({StatementType::INSERT, {2, "jane", "doe"}});

    table.exec({StatementType::SELECT});

    return 0;
}
