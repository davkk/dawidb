#include "row.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <span>

void Row::serialize_into(const std::span<char> dest) {
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

void Row::deserialize_from(const std::span<char> src) {
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
