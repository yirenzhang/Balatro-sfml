#include "ShopRestock.hpp"

#include <cstdlib>

std::vector<std::string> ShopRestock::PickIds(
    const std::vector<std::string>& pool,
    int count,
    const PickIndexFn& pickIndex
) {
    std::vector<std::string> selected;
    if (pool.empty() || count <= 0 || !pickIndex) {
        return selected;
    }

    selected.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        const std::size_t idx = pickIndex(pool.size()) % pool.size();
        selected.push_back(pool[idx]);
    }
    return selected;
}

std::vector<std::string> ShopRestock::PickIdsWithStdRand(
    const std::vector<std::string>& pool,
    int count
) {
    return PickIds(pool, count, [](std::size_t bound) {
        return static_cast<std::size_t>(std::rand() % static_cast<int>(bound));
    });
}
