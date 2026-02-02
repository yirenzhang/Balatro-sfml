#pragma once
#include <vector>
#include <string>
#include "../Objects/Card.hpp"

// [修复] 前置声明 CardArea，告诉编译器这是一个类
class CardArea;

// 触发时机 (还原 Lua 的上下文逻辑)
enum class TriggerType {
    Individual,     // [新] 针对每一张打出的牌触发 (context.individual)
    Global,         // [新] 针对整手牌的全局触发 (context.joker_main)
    OnDiscard,      // 弃牌时
    OnShopEntry,    // 商店
    // ... 其他 ...
};

// 效果反馈
struct EffectResult {
    bool triggered = false;
    int chips_add = 0;
    int mult_add = 0;
    float x_mult = 1.0f;
    int dollars_add = 0;
    std::string message;
};

// 核心上下文
struct EffectContext {
    TriggerType trigger;
    
    // 全局信息
    std::vector<Card*> scoring_cards; // 所有计分牌
    CardArea* joker_area = nullptr;   // 引用 Joker 区 (用于 Abstract Joker 统计数量)

    // [关键新增] 当前正在评估的目标卡牌
    // 当 trigger == Individual 时，这个指针指向当前遍历到的那张扑克牌 (context.other_card)
    Card* other_card = nullptr; 

    int current_chips = 0;
    int current_mult = 0;
};