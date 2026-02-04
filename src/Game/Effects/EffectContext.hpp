#pragma once
#include <vector>
#include <string>
#include "../Objects/Card.hpp"

class CardArea;

/**
 * 触发时机枚举
 * * 用于区分效果是在何时被调用的。
 */
enum class TriggerType {
    Individual,     // 结算打出的牌 (每张牌独立触发)
    HeldInHand,     // [新增] 结算手中的牌 (例如钢铁牌、男爵)
    Global,         // 全局结算 (Joker 汇总)
    OnDiscard,      // 弃牌时
    OnShopEntry,    // 进入商店时
};

// ... (EffectResult 和 EffectContext 保持不变)
struct EffectResult {
    bool triggered = false;
    int chips_add = 0;
    int mult_add = 0;
    float x_mult = 1.0f;
    int dollars_add = 0;
    std::string message;
};

struct EffectContext {
    TriggerType trigger;
    
    std::vector<Card*> scoring_cards; 
    CardArea* joker_area = nullptr;
    
    // [新增] 引用手牌区，方便计算“手中持有的牌”
    CardArea* hand_area = nullptr;   

    Card* other_card = nullptr; 

    int current_chips = 0;
    int current_mult = 0;
};