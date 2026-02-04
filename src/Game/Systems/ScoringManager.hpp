#pragma once
#include <vector>
#include <iostream>
#include "HandEvaluator.hpp"
#include "../Effects/EffectContext.hpp"
#include "../Effects/IEffect.hpp"
#include "../Objects/CardArea.hpp"

struct ScoreSummary {
    long long final_score = 0;
    int final_chips = 0;
    int final_mult = 0;
    int dollars = 0;
    std::vector<std::string> trigger_log;
};

class ScoringManager {
public:
    /**
     * 计算最终得分
     * * 包含三个阶段：打出牌结算 -> 手牌结算 -> Joker 全局结算
     * * @param handResult 牌型评估结果
     * @param handArea 手牌区域 (新增参数，用于支持 Held in Hand 效果)
     * @param jokerArea Joker 区域
     */
    static ScoreSummary CalculateFinalScore(
        const HandResult& handResult, 
        CardArea* handArea, 
        CardArea* jokerArea
    ) {
        ScoreSummary summary;
        int currentChips = handResult.base_chips;
        int currentMult = handResult.base_mult;

        summary.trigger_log.push_back("Base: " + std::to_string(currentChips) + " x " + std::to_string(currentMult));

        // 初始化上下文
        EffectContext ctx;
        ctx.scoring_cards = handResult.scoring_cards;
        ctx.hand_area = handArea;
        ctx.joker_area = jokerArea;

        // --- 阶段 0: 基础筹码累加 ---
        for (auto* card : handResult.scoring_cards) {
            currentChips += card->getChips();
        }

        // --- 阶段 1: 结算打出的牌 (Scoring Cards) ---
        // 比如：玻璃牌、黄金牌在打出时触发
        if (jokerArea) {
            ctx.trigger = TriggerType::Individual;
            for (auto* playingCard : handResult.scoring_cards) {
                ctx.other_card = playingCard; 
                
                // 1.1 检查 Joker 是否对这张牌有反应 (例如：Greedy Joker)
                for (auto& joker : jokerArea->getCards()) {
                    processEffect(joker, ctx, currentChips, currentMult, summary, "Joker");
                }

                // 1.2 (未来扩展) 检查卡牌自身的增强效果 (Enhancement)
                // if (playingCard->getEffect()) ...
            }
        }

        // --- 阶段 2: 结算手中的牌 (Held in Hand) ---
        // 比如：钢铁牌 (Steel)、男爵 (Baron)
        if (handArea && jokerArea) {
            ctx.trigger = TriggerType::HeldInHand;
            
            // 遍历手牌中 **没有被选中打出** 的牌
            // 简单做法是遍历所有手牌，排除掉 scoring_cards 里的
            // 但 Balatro 逻辑是：只要在手牌区就是 Held，打出去的牌此时已不在手牌区(视觉上)，
            // 但我们在代码里通常还没移除。
            // 这里我们遍历 handArea 的所有牌，如果是 selected 的，通常意味着正在被打出，不应算作 Held。
            
            for (auto& cardPtr : handArea->getCards()) {
                Card* card = cardPtr.get();
                if (card->isSelected()) continue; // 被选中的牌(正在打出)不享受钢铁加成

                ctx.other_card = card;

                // 2.1 检查 Joker 对手牌的反应 (例如：Baron, Mime)
                for (auto& joker : jokerArea->getCards()) {
                    processEffect(joker, ctx, currentChips, currentMult, summary, "Held");
                }
                
                // 2.2 (未来扩展) 检查卡牌自身的钢铁效果
            }
        }

        // --- 阶段 3: 全局 Joker 结算 (Global) ---
        // 比如：+4 Mult, x3 Mult
        if (jokerArea) {
            ctx.trigger = TriggerType::Global;
            ctx.other_card = nullptr; 
            ctx.current_chips = currentChips; 
            ctx.current_mult = currentMult; // 某些 Joker 需要知道当前倍率

            for (auto& joker : jokerArea->getCards()) {
                processEffect(joker, ctx, currentChips, currentMult, summary, "Global");
            }
        }

        summary.final_chips = currentChips;
        summary.final_mult = currentMult;
        summary.final_score = (long long)currentChips * (long long)currentMult;
        
        return summary;
    }

    /**
     * 计算弃牌效果 (On Discard)
     * * 遍历被弃掉的牌，检查 Joker 是否触发效果（如返利、销毁等）。
     * * @param discardedCards 玩家选择弃掉的牌
     * @param jokerArea Joker 区域
     */
    static ScoreSummary CalculateDiscardEffect(
        const std::vector<Card*>& discardedCards, 
        CardArea* jokerArea
    ) {
        ScoreSummary summary;
        EffectContext ctx;
        ctx.joker_area = jokerArea;
        
        // 弃牌上下文比较特殊：scoring_cards 这里指代“被弃掉的牌”
        ctx.scoring_cards = discardedCards; 

        if (!jokerArea) return summary;

        // --- 阶段 1: 针对每一张弃牌触发 (Individual) ---
        // 例如：挂牌 (Mail-In Rebate) - "每弃掉一张..."
        ctx.trigger = TriggerType::OnDiscard;
        
        for (auto* card : discardedCards) {
            ctx.other_card = card; // 当前正在被弃掉的这张牌

            for (auto& joker : jokerArea->getCards()) {
                auto effect = joker->getEffect();
                if (effect) {
                    auto res = effect->Calculate(*joker, ctx);
                    if (res && res->triggered) {
                        // 处理金钱奖励
                        if (res->dollars_add > 0) {
                            summary.dollars += res->dollars_add;
                            summary.trigger_log.push_back(
                                "Discard (" + joker->getAbilityName() + "): +$" + std::to_string(res->dollars_add)
                            );
                        }
                        
                        // 处理倍率/筹码 (极少数 Joker 会在弃牌时加分，但架构上支持)
                        if (res->chips_add > 0) summary.final_chips += res->chips_add;
                        if (res->mult_add > 0) summary.final_mult += res->mult_add;
                    }
                }
            }
        }

        // --- 阶段 2: 针对整个弃牌动作触发 (Global) ---
        // 例如：贸易卡 (Trading Card) - "如果弃牌数量为 1..."
        // 实现逻辑：
        // ctx.trigger = TriggerType::OnDiscardAction; // 需要在 TriggerType 扩展
        // ctx.other_card = nullptr;
        // ... 遍历 Joker ...

        return summary;
    }

private:
    /**
     * 统一处理单个效果的计算与应用
     * * 避免代码重复，处理 x_mult 逻辑
     */
    static void processEffect(
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
        if (res && res->triggered) {
            // 应用加法效果
            if (res->chips_add > 0) chips += res->chips_add;
            if (res->mult_add > 0) mult += res->mult_add;
            
            // 应用乘法效果 (x Mult)
            if (res->x_mult > 1.0f) {
                mult = (int)(mult * res->x_mult);
                summary.trigger_log.push_back(
                    prefix + " (" + sourceCard->getAbilityName() + "): X" + std::to_string(res->x_mult)
                );
            } else {
                summary.trigger_log.push_back(
                    prefix + " (" + sourceCard->getAbilityName() + "): " + res->message
                );
            }
        }
    }
};