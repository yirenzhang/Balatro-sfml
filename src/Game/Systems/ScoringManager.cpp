#include "ScoringManager.hpp"

#include "../Effects/IEffect.hpp"
#include "CardSnapshotUtils.hpp"

ScoreSummary ScoringManager::CalculateFinalScore(
    int baseChips,
    int baseMult,
    const std::vector<CardSnapshot>& scoringCards,
    CardArea* handArea,
    CardArea* jokerArea
) {
    ScoreSummary summary;
    int currentChips = baseChips;
    int currentMult = baseMult;

    // 记录基础值，便于调试触发链条时回溯最终来源。
    summary.trigger_log.push_back("Base: " + std::to_string(currentChips) + " x " + std::to_string(currentMult));

    EffectContext ctx;
    ctx.scoring_snapshots = scoringCards;
    ctx.hand_area = handArea;
    ctx.joker_area = jokerArea;

    // 先叠加计分牌基础筹码，再进入效果链，保持与玩法结算顺序一致。
    for (const auto& card : scoringCards) {
        currentChips += card.chips;
    }

    // 第一阶段：逐张计分牌触发 Individual 效果。
    if (jokerArea) {
        ctx.trigger = TriggerType::Individual;
        for (const auto& playingCard : scoringCards) {
            ctx.other_card_snapshot = playingCard;
            ctx.has_other_card_snapshot = true;
            for (auto& joker : jokerArea->getCards()) {
                processEffect(joker, ctx, currentChips, currentMult, summary, "Joker");
            }
        }
    }

    // 第二阶段：对“未出牌手持牌”触发 HeldInHand 效果。
    if (handArea && jokerArea) {
        ctx.trigger = TriggerType::HeldInHand;
        auto heldCards = CardSnapshotUtils::BuildHeldInHand(*handArea);
        for (const auto& heldCard : heldCards) {
            ctx.other_card_snapshot = heldCard;
            ctx.has_other_card_snapshot = true;
            for (auto& joker : jokerArea->getCards()) {
                processEffect(joker, ctx, currentChips, currentMult, summary, "Held");
            }
        }
    }

    // 第三阶段：执行全局效果，作为本次结算的最终修正层。
    if (jokerArea) {
        ctx.trigger = TriggerType::Global;
        ctx.has_other_card_snapshot = false;
        ctx.current_chips = currentChips;
        ctx.current_mult = currentMult;

        for (auto& joker : jokerArea->getCards()) {
            processEffect(joker, ctx, currentChips, currentMult, summary, "Global");
        }
    }

    summary.final_chips = currentChips;
    summary.final_mult = currentMult;
    summary.final_score = static_cast<long long>(currentChips) * static_cast<long long>(currentMult);
    return summary;
}

ScoreSummary ScoringManager::CalculateDiscardEffect(
    const std::vector<CardSnapshot>& discardedCards,
    CardArea* jokerArea
) {
    ScoreSummary summary;
    EffectContext ctx;
    ctx.joker_area = jokerArea;
    ctx.scoring_snapshots = discardedCards;

    if (!jokerArea) return summary;

    // 弃牌链路独立于出牌链路，避免两类触发条件互相污染。
    ctx.trigger = TriggerType::OnDiscard;
    for (const auto& card : discardedCards) {
        ctx.other_card_snapshot = card;
        ctx.has_other_card_snapshot = true;

        for (auto& joker : jokerArea->getCards()) {
            auto effect = joker->getEffect();
            if (!effect) continue;

            auto res = effect->Calculate(*joker, ctx);
            if (!res || !res->triggered) continue;

            if (res->dollars_add > 0) {
                summary.dollars += res->dollars_add;
                summary.trigger_log.push_back(
                    "Discard (" + joker->getAbilityName() + "): +$" + std::to_string(res->dollars_add)
                );
            }

            if (res->chips_add > 0) summary.final_chips += res->chips_add;
            if (res->mult_add > 0) summary.final_mult += res->mult_add;
        }
    }

    return summary;
}

void ScoringManager::processEffect(
    const std::shared_ptr<Card>& sourceCard,
    const EffectContext& ctx,
    int& chips,
    int& mult,
    ScoreSummary& summary,
    const std::string& prefix
) {
    auto effect = sourceCard->getEffect();
    if (!effect) return;

    auto res = effect->Calculate(*sourceCard, ctx);
    if (!res || !res->triggered) return;

    if (res->chips_add > 0) chips += res->chips_add;
    if (res->mult_add > 0) mult += res->mult_add;

    // x_mult 属于“后乘”修正，必须在加法修正后应用以符合设计语义。
    if (res->x_mult > 1.0f) {
        mult = static_cast<int>(mult * res->x_mult);
        summary.trigger_log.push_back(
            prefix + " (" + sourceCard->getAbilityName() + "): X" + std::to_string(res->x_mult)
        );
    } else {
        summary.trigger_log.push_back(
            prefix + " (" + sourceCard->getAbilityName() + "): " + res->message
        );
    }
}
