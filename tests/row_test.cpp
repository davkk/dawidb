#include "row.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(RowTest, SerializeDeserialize) {
    Row initial{420, {"hello"}, {"world"}};

    Row buffer;
    initial.serialize(&buffer);

    auto final{Row::from(&buffer)};

    ASSERT_EQ(initial.id, final.id);
    ASSERT_TRUE("usernames equal" && std::strcmp(initial.username, final.username) == 0);
    ASSERT_TRUE("emails equal" && std::strcmp(initial.email, final.email) == 0);
}
