#include "RunFlow.hpp"

void RunFlow::ApplyDiscard(GameContext& ctx, const ScoreSummary& discardSummary) {
    // 先应用收益再扣次数，便于出现边界问题时保留收益日志一致性。
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
    // 统一先落地本次得分与手数消耗，再做迁移判定，避免分支遗漏。
    ctx.currentScore += summary.final_score;
    if (ctx.handsLeft > 0) {
        --ctx.handsLeft;
    }

    // 达标优先于手数耗尽判定，确保“最后一手达标”不会误判失败。
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
