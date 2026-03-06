#pragma once

#include <string>
#include <vector>

#include "../Systems/CardSnapshot.hpp"

class CardArea;

/**
 * 效果触发阶段。
 *
 * 将触发阶段显式化的目的是减少效果实现中的隐式约定，
 * 让同一效果在不同结算链路上行为可预测。
 */
enum class TriggerType {
    Individual,
    HeldInHand,
    Global,
    OnDiscard,
    OnShopEntry,
};

/**
 * 单次效果计算结果。
 */
struct EffectResult {
    bool triggered = false;
    int chips_add = 0;
    int mult_add = 0;
    float x_mult = 1.0f;
    int dollars_add = 0;
    std::string message;
};

/**
 * 效果计算上下文。
 *
 * 保留快照与区域指针是为了让效果可读取必要玩法信息，
 * 同时避免直接依赖渲染对象。
 */
struct EffectContext {
    TriggerType trigger;

    std::vector<CardSnapshot> scoring_snapshots;
    CardArea* joker_area = nullptr;
    CardArea* hand_area = nullptr;

    CardSnapshot other_card_snapshot;
    bool has_other_card_snapshot = false;

    int current_chips = 0;
    int current_mult = 0;
};
