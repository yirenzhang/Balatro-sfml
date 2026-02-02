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
    std::vector<std::string> trigger_log;
};

class ScoringManager {
public:
    static ScoreSummary CalculateFinalScore(
        const HandResult& handResult, 
        CardArea* jokerArea
    ) {
        ScoreSummary summary;
        int currentChips = handResult.base_chips;
        int currentMult = handResult.base_mult;

        summary.trigger_log.push_back("Base: " + std::to_string(currentChips) + " x " + std::to_string(currentMult));

        EffectContext ctx;
        ctx.scoring_cards = handResult.scoring_cards;
        ctx.joker_area = jokerArea;

        // Phase 1: Card Stat
        for (auto* card : handResult.scoring_cards) {
            currentChips += card->getChips();
        }

        // Phase 2: Individual Context
        if (jokerArea) {
            ctx.trigger = TriggerType::Individual;
            for (auto* playingCard : handResult.scoring_cards) {
                ctx.other_card = playingCard; 
                for (auto& joker : jokerArea->getCards()) {
                    auto effect = joker->getEffect();
                    if (effect) {
                        auto res = effect->Calculate(*joker, ctx);
                        if (res && res->triggered) {
                            currentChips += res->chips_add;
                            currentMult += res->mult_add;
                            summary.trigger_log.push_back(
                                "Indiv (" + joker->getAbilityName() + "): " + res->message
                            );
                        }
                    }
                }
            }
        }

        // Phase 3: Global Context
        if (jokerArea) {
            ctx.trigger = TriggerType::Global;
            ctx.other_card = nullptr; 
            ctx.current_chips = currentChips; 

            for (auto& joker : jokerArea->getCards()) {
                auto effect = joker->getEffect();
                if (effect) {
                    auto res = effect->Calculate(*joker, ctx);
                    if (res && res->triggered) {
                        if (res->chips_add > 0) currentChips += res->chips_add;
                        if (res->mult_add > 0) currentMult += res->mult_add;
                        
                        if (res->x_mult > 1.0f) {
                            // [修复] 直接计算，不声明未使用的变量
                            currentMult = (int)(currentMult * res->x_mult);
                            summary.trigger_log.push_back(
                                "Global (" + joker->getAbilityName() + "): X" + std::to_string(res->x_mult)
                            );
                        } else {
                            summary.trigger_log.push_back(
                                "Global (" + joker->getAbilityName() + "): " + res->message
                            );
                        }
                    }
                }
            }
        }

        summary.final_chips = currentChips;
        summary.final_mult = currentMult;
        summary.final_score = (long long)currentChips * (long long)currentMult;
        
        return summary;
    }
};