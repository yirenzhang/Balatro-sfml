#pragma once

#include <vector>

#include "../Objects/Card.hpp"
#include "../Objects/CardArea.hpp"
#include "CardSnapshot.hpp"

namespace CardSnapshotUtils {

/**
 * 从卡牌对象构建快照。
 *
 * @param card 源卡牌
 * @return 牌面快照
 */
inline CardSnapshot FromCard(const Card& card) {
    return CardSnapshot{
        .suit = card.getSuit(),
        .rank = card.getRank(),
        .chips = card.getChips(),
        .selected = card.isSelected(),
        .source = &card
    };
}

/**
 * 从裸指针列表构建快照列表。
 *
 * @param cards 卡牌指针列表
 * @return 快照列表
 */
inline std::vector<CardSnapshot> BuildFromCards(const std::vector<Card*>& cards) {
    std::vector<CardSnapshot> snapshots;
    snapshots.reserve(cards.size());
    for (auto* card : cards) {
        if (!card) continue;
        snapshots.push_back(FromCard(*card));
    }
    return snapshots;
}

/**
 * 构建“手持但未选中”快照列表。
 *
 * @param area 手牌区域
 * @return 快照列表
 */
inline std::vector<CardSnapshot> BuildHeldInHand(CardArea& area) {
    std::vector<CardSnapshot> snapshots;
    snapshots.reserve(area.getCards().size());
    for (const auto& cardPtr : area.getCards()) {
        if (!cardPtr || cardPtr->isSelected()) continue;
        snapshots.push_back(FromCard(*cardPtr));
    }
    return snapshots;
}

/**
 * 构建“已选中”快照列表。
 *
 * @param area 手牌区域
 * @return 快照列表
 */
inline std::vector<CardSnapshot> BuildSelected(CardArea& area) {
    std::vector<CardSnapshot> snapshots;
    snapshots.reserve(area.getCards().size());
    for (const auto& cardPtr : area.getCards()) {
        if (!cardPtr || !cardPtr->isSelected()) continue;
        snapshots.push_back(FromCard(*cardPtr));
    }
    return snapshots;
}

} // namespace CardSnapshotUtils
