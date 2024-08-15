#include "row.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <span>

void Row::serialize_into(const std::span<char> dest) {
    assert(dest.size() == ROW_SIZE);
    std::span<char> span;

    span = dest.subspan(ID_OFFSET, ID_SIZE);
    assert(span.size() == ID_SIZE);
    std::memcpy(span.data(), &id, ID_SIZE);

    span = dest.subspan(USERNAME_OFFSET, USERNAME_SIZE);
    assert(span.size() == USERNAME_SIZE);
    std::copy(username.begin(), username.end(), span.begin());

    span = dest.subspan(EMAIL_OFFSET, EMAIL_SIZE);
    assert(span.size() == EMAIL_SIZE);
    std::copy(email.begin(), email.end(), span.begin());
}

void Row::deserialize_from(const std::span<char> src) {
    assert(src.size() == ROW_SIZE);
    std::span<char> span;

    span = src.subspan(ID_OFFSET, ID_SIZE);
    assert(span.size() == ID_SIZE);
    std::memcpy(&id, span.data(), ID_SIZE);

    span = src.subspan(USERNAME_OFFSET, USERNAME_SIZE);
    assert(span.size() == USERNAME_SIZE);
    std::copy(span.begin(), span.end(), username.begin());

    span = src.subspan(EMAIL_OFFSET, EMAIL_SIZE);
    assert(span.size() == EMAIL_SIZE);
    std::copy(span.begin(), span.end(), email.begin());
}
