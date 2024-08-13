#include <cstddef>
#include <span>

#include "pager.hpp"
#include "parser.hpp"
#include "with_error.hpp"

class Table {
public:
    void exec(Statement&& statement);

private:
    size_t num_rows{0};
    std::array<Page, MAX_PAGES> pages{};

    WithError<std::span<char>, PagerError> find_slot(size_t row_num);
};
