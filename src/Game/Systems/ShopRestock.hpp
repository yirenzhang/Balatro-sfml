#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace ShopRestock {

using PickIndexFn = std::function<std::size_t(std::size_t bound)>;

/**
 * 从商品池中按策略选择若干 ID。
 *
 * @param pool 商品池
 * @param count 选择数量
 * @param pickIndex 索引策略
 * @return 选中的 ID 列表
 */
std::vector<std::string> PickIds(
    const std::vector<std::string>& pool,
    int count,
    const PickIndexFn& pickIndex
);

/**
 * 使用标准随机数生成补货 ID。
 *
 * @param pool 商品池
 * @param count 选择数量
 * @return 选中的 ID 列表
 */
std::vector<std::string> PickIdsWithStdRand(
    const std::vector<std::string>& pool,
    int count
);

} // namespace ShopRestock
