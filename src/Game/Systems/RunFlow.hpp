#pragma once

#include "../Core/GameContext.hpp"
#include "ScoringManager.hpp"

enum class RoundTransition {
    StayInRun,
    ToShop,
    GameOver
};

namespace RunFlow {

/**
 * 应用弃牌阶段结算结果。
 *
 * @param ctx 上下文
 * @param discardSummary 弃牌结算结果
 */
void ApplyDiscard(GameContext& ctx, const ScoreSummary& discardSummary);

/**
 * 应用出牌结算并返回状态迁移决策。
 *
 * 将迁移决策抽离后，状态层只负责“执行”，
 * 规则层负责“判定”，便于测试与维护。
 *
 * @param ctx 上下文
 * @param summary 出牌结算结果
 * @param clearReward 过关奖励
 * @param targetScale 目标分成长系数
 * @return 迁移结果
 */
RoundTransition ApplyPlay(
    GameContext& ctx,
    const ScoreSummary& summary,
    int clearReward = 5,
    float targetScale = 1.5f
);

} // namespace RunFlow
