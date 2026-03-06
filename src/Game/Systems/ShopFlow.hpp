#pragma once

#include "../Core/GameContext.hpp"

struct ShopPurchaseDecision {
    bool affordable = false;
    bool requiresReplace = false;
};

namespace ShopFlow {

constexpr int DEFAULT_JOKER_LIMIT = 5;
constexpr int DEFAULT_REROLL_COST = 5;

/**
 * 尝试扣除指定金额。
 *
 * @param ctx 上下文
 * @param amount 扣费金额
 * @return 扣费是否成功
 */
bool TrySpend(GameContext& ctx, int amount);

/**
 * 尝试执行商店刷新扣费。
 *
 * @param ctx 上下文
 * @param rerollCost 刷新费用
 * @return 扣费是否成功
 */
bool TryReroll(GameContext& ctx, int rerollCost = DEFAULT_REROLL_COST);

/**
 * 评估购买可行性。
 *
 * @param ctx 上下文
 * @param cost 商品价格
 * @param jokerLimit Joker 槽位上限
 * @return 购买判定结果
 */
ShopPurchaseDecision EvaluatePurchase(
    const GameContext& ctx,
    int cost,
    int jokerLimit = DEFAULT_JOKER_LIMIT
);

} // namespace ShopFlow
