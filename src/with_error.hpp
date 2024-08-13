#pragma once

#include <optional>
#include <utility>

template <typename Type, typename Error>
using WithError = std::pair<Type, std::optional<Error>>;
