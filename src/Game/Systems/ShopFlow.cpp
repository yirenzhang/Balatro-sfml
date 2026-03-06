#include "ShopFlow.hpp"

#include "../Objects/CardArea.hpp"

bool ShopFlow::TrySpend(GameContext& ctx, int amount) {
    // 拒绝负数金额，避免调用方错误造成“反向加钱”漏洞。
    if (amount < 0) return false;
    if (ctx.money < amount) return false;
    ctx.money -= amount;
    return true;
}

bool ShopFlow::TryReroll(GameContext& ctx, int rerollCost) {
    // 刷新同样走统一扣费，避免商店分支出现双重标准。
    return TrySpend(ctx, rerollCost);
}

ShopPurchaseDecision ShopFlow::EvaluatePurchase(
    const GameContext& ctx,
    int cost,
    int jokerLimit
) {
    ShopPurchaseDecision decision;
    // 先做输入合法性防御，避免 UI 异常输入污染购买状态。
    if (cost < 0 || jokerLimit <= 0 || !ctx.hasJokerArea()) {
        return decision;
    }

    if (ctx.money < cost) {
        return decision;
    }

    // 预算通过后再判断是否需要替换，减少 UI 层条件重复。
    decision.affordable = true;
    decision.requiresReplace = static_cast<int>(ctx.jokerArea().getCards().size()) >= jokerLimit;
    return decision;
}
