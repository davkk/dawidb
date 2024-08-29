#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "cursor.hpp"
#include "pager.hpp"
#include "parser.hpp"

enum TableErrorCode {
    DUPLICATE_KEY = 1
};

struct TableError {
    TableErrorCode code;
    std::string message;
};

struct Table {
    Pager pager;
    size_t root_page_num{0};

    explicit Table(std::fstream &file);

    Table(const Table &) = delete;
    Table(Table &&) = delete;
    Table &operator=(const Table &) = delete;
    Table &operator=(Table &&) = delete;

    void show();
    WithError<std::optional<std::vector<Row>>, TableError> exec(
        Statement &&statement
    );

    void advance_cursor(Cursor &cursor);

    Cursor begin();
    Cursor find(uint32_t key);
};
