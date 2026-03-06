#include "ShopFlow.hpp"

#include "../Objects/CardArea.hpp"

bool ShopFlow::TrySpend(GameContext& ctx, int amount) {
    if (amount < 0) return false;
    if (ctx.money < amount) return false;
    ctx.money -= amount;
    return true;
}

bool ShopFlow::TryReroll(GameContext& ctx, int rerollCost) {
    return TrySpend(ctx, rerollCost);
}

ShopPurchaseDecision ShopFlow::EvaluatePurchase(
    const GameContext& ctx,
    int cost,
    int jokerLimit
) {
    ShopPurchaseDecision decision;
    if (cost < 0 || jokerLimit <= 0 || !ctx.hasJokerArea()) {
        return decision;
    }

    if (ctx.money < cost) {
        return decision;
    }

    decision.affordable = true;
    decision.requiresReplace = static_cast<int>(ctx.jokerArea().getCards().size()) >= jokerLimit;
    return decision;
}
