#include "RunFlow.hpp"

void RunFlow::ApplyDiscard(GameContext& ctx, const ScoreSummary& discardSummary) {
    if (discardSummary.dollars > 0) {
        ctx.money += discardSummary.dollars;
    }
    if (ctx.discardsLeft > 0) {
        --ctx.discardsLeft;
    }
}

RoundTransition RunFlow::ApplyPlay(
    GameContext& ctx,
    const ScoreSummary& summary,
    int clearReward,
    float targetScale
) {
    ctx.currentScore += summary.final_score;
    if (ctx.handsLeft > 0) {
        --ctx.handsLeft;
    }

    if (ctx.currentScore >= ctx.targetScore) {
        ctx.money += clearReward;
        ctx.targetScore = static_cast<long long>(ctx.targetScore * targetScale);
        return RoundTransition::ToShop;
    }

    if (ctx.handsLeft <= 0) {
        return RoundTransition::GameOver;
    }

    return RoundTransition::StayInRun;
}
