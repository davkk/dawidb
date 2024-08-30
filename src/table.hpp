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

    explicit Table(std::fstream& file);

    Table(const Table&) = delete;
    Table(Table&&) = delete;
    auto& operator=(const Table&) = delete;
    auto& operator=(Table&&) = delete;

    auto show() -> void;
    auto exec(Statement&& statement) -> WithError<std::optional<std::vector<Row>>, TableError>;

    auto advance_cursor(Cursor& cursor) -> void;

    auto begin() -> Cursor;
    auto find(uint32_t key) -> Cursor;
};
