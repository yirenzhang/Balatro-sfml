#include "ShopRestock.hpp"

#include <cstdlib>

std::vector<std::string> ShopRestock::PickIds(
    const std::vector<std::string>& pool,
    int count,
    const PickIndexFn& pickIndex
) {
    std::vector<std::string> selected;
    // 输入校验集中在入口，避免调用方为每次补货重复写防御代码。
    if (pool.empty() || count <= 0 || !pickIndex) {
        return selected;
    }

    selected.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        // 通过取模兜底异常索引，保证策略函数越界时仍能安全运行。
        const std::size_t idx = pickIndex(pool.size()) % pool.size();
        selected.push_back(pool[idx]);
    }
    return selected;
}

std::vector<std::string> ShopRestock::PickIdsWithStdRand(
    const std::vector<std::string>& pool,
    int count
) {
    // 当前采用标准随机即可满足商店波动需求。
    // TODO: 若后续需要回放，可在此替换为可注入随机源实现。
    return PickIds(pool, count, [](std::size_t bound) {
        return static_cast<std::size_t>(std::rand() % static_cast<int>(bound));
    });
}
