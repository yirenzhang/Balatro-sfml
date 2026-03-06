#pragma once
#include <vector>
#include <string>
#include <memory>
#include "CardSnapshot.hpp"
#include "../Effects/EffectContext.hpp"
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
     * 计算出牌最终得分。
     *
     * @param baseChips 基础筹码
     * @param baseMult 基础倍率
     * @param scoringCards 计分牌快照
     * @param handArea 手牌区
     * @param jokerArea Joker 区
     * @return 结算结果
     */
    static ScoreSummary CalculateFinalScore(
        int baseChips,
        int baseMult,
        const std::vector<CardSnapshot>& scoringCards,
        CardArea* handArea,
        CardArea* jokerArea
    );

    /**
     * 计算弃牌阶段效果。
     *
     * @param discardedCards 弃牌快照
     * @param jokerArea Joker 区
     * @return 结算结果
     */
    static ScoreSummary CalculateDiscardEffect(
        const std::vector<CardSnapshot>& discardedCards,
        CardArea* jokerArea
    );

private:
    static void processEffect(
        const std::shared_ptr<Card>& sourceCard, 
        const EffectContext& ctx, 
        int& chips, 
        int& mult, 
        ScoreSummary& summary,
        const std::string& prefix
    );
};
