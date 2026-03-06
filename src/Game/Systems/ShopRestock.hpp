#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace ShopRestock {

using PickIndexFn = std::function<std::size_t(std::size_t bound)>;

std::vector<std::string> PickIds(
    const std::vector<std::string>& pool,
    int count,
    const PickIndexFn& pickIndex
);

std::vector<std::string> PickIdsWithStdRand(
    const std::vector<std::string>& pool,
    int count
);

} // namespace ShopRestock
