#pragma once

#include "../Core/GameContext.hpp"

struct ShopPurchaseDecision {
    bool affordable = false;
    bool requiresReplace = false;
};

namespace ShopFlow {

constexpr int DEFAULT_JOKER_LIMIT = 5;
constexpr int DEFAULT_REROLL_COST = 5;

bool TrySpend(GameContext& ctx, int amount);

bool TryReroll(GameContext& ctx, int rerollCost = DEFAULT_REROLL_COST);

ShopPurchaseDecision EvaluatePurchase(
    const GameContext& ctx,
    int cost,
    int jokerLimit = DEFAULT_JOKER_LIMIT
);

} // namespace ShopFlow
