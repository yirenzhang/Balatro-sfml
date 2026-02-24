#pragma once
#include <vector>
#include <string>
#include <memory>
#include "HandEvaluator.hpp"
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
    static ScoreSummary CalculateFinalScore(
        int baseChips,
        int baseMult,
        const std::vector<CardSnapshot>& scoringCards,
        CardArea* handArea,
        CardArea* jokerArea
    );

    static ScoreSummary CalculateFinalScore(
        const HandResult& handResult, 
        CardArea* handArea, 
        CardArea* jokerArea
    );

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
