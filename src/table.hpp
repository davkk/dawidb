#include <cstddef>

#include "pager.hpp"
#include "parser.hpp"

struct Table {
    size_t num_rows;
    Pager pager;

    explicit Table(std::fstream& file);
    ~Table();

    void exec(Statement&& statement);
};
