#pragma once

#include <vector>

#include "../Objects/Card.hpp"
#include "../Objects/CardArea.hpp"
#include "CardSnapshot.hpp"

namespace CardSnapshotUtils {

inline CardSnapshot FromCard(const Card& card) {
    return CardSnapshot{
        .suit = card.getSuit(),
        .rank = card.getRank(),
        .chips = card.getChips(),
        .selected = card.isSelected(),
        .source = &card
    };
}

inline std::vector<CardSnapshot> BuildFromCards(const std::vector<Card*>& cards) {
    std::vector<CardSnapshot> snapshots;
    snapshots.reserve(cards.size());
    for (auto* card : cards) {
        if (!card) continue;
        snapshots.push_back(FromCard(*card));
    }
    return snapshots;
}

inline std::vector<CardSnapshot> BuildHeldInHand(CardArea& area) {
    std::vector<CardSnapshot> snapshots;
    snapshots.reserve(area.getCards().size());
    for (const auto& cardPtr : area.getCards()) {
        if (!cardPtr || cardPtr->isSelected()) continue;
        snapshots.push_back(FromCard(*cardPtr));
    }
    return snapshots;
}

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
