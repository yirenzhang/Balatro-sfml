#pragma once

#include "../Core/GameContext.hpp"
#include "ScoringManager.hpp"

enum class RoundTransition {
    StayInRun,
    ToShop,
    GameOver
};

namespace RunFlow {

void ApplyDiscard(GameContext& ctx, const ScoreSummary& discardSummary);

RoundTransition ApplyPlay(
    GameContext& ctx,
    const ScoreSummary& summary,
    int clearReward = 5,
    float targetScale = 1.5f
);

} // namespace RunFlow
