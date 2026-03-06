#include "CardArea.hpp"

#include <algorithm>
#include <cmath>

CardArea::CardArea(float x, float y, float w, float h, LayoutType type)
    : m_bounds(x, y, w, h), m_layoutType(type) {
    m_debugBox.setPosition(x, y);
    m_debugBox.setSize(sf::Vector2f(w, h));
    m_debugBox.setFillColor(sf::Color::Transparent);
    m_debugBox.setOutlineColor(sf::Color(255, 255, 255, 50));
    m_debugBox.setOutlineThickness(1.0f);
}

void CardArea::addCard(std::shared_ptr<Card> card) {
    m_cards.push_back(std::move(card));
}

void CardArea::removeCard(int index) {
    if (index >= 0 && index < static_cast<int>(m_cards.size())) {
        m_cards.erase(m_cards.begin() + index);
        alignCards();
    }
}

std::shared_ptr<Card> CardArea::takeCard(Card* cardPtr) {
    auto it = std::find_if(
        m_cards.begin(),
        m_cards.end(),
        [cardPtr](const std::shared_ptr<Card>& ptr) {
            return ptr.get() == cardPtr;
        }
    );

    if (it == m_cards.end()) {
        return nullptr;
    }

    std::shared_ptr<Card> card = *it;
    m_cards.erase(it);
    alignCards();
    return card;
}

void CardArea::alignCards() {
    if (m_cards.empty()) return;

    float cardW = Card::DECK_WIDTH * 3.0f;
    cardW = m_cards[0]->getGlobalBounds().width;
    if (cardW < 10.0f) {
        cardW = Card::DECK_WIDTH * 3.0f;
    }

    const float centerX = m_bounds.left + m_bounds.width / 2.0f;
    const float centerY = m_bounds.top + m_bounds.height / 2.0f;
    const int count = static_cast<int>(m_cards.size());

    float maxSpacing = cardW * 1.1f;
    if (m_layoutType == LayoutType::Fan) {
        maxSpacing = cardW * 0.5f;
    }

    const float currentTotalWidth = (count - 1) * maxSpacing + cardW;
    const float availableWidth = m_bounds.width - 20.0f;

    float stepX = maxSpacing;
    if (currentTotalWidth > availableWidth && count > 1) {
        stepX = (availableWidth - cardW) / static_cast<float>(count - 1);
    }

    if (m_layoutType == LayoutType::Fan) {
        const float finalTotalWidth = (count - 1) * stepX;
        const float startX = centerX - finalTotalWidth / 2.0f;
        for (int i = 0; i < count; ++i) {
            const float x = startX + i * stepX;
            const float relativeX = (i - (count - 1) / 2.0f);
            const float yOffset = std::abs(relativeX) * std::abs(relativeX) * 2.0f;
            m_cards[i]->setTargetPosition(x, centerY + yOffset + 20.0f);
        }
    } else if (m_layoutType == LayoutType::Row) {
        const float finalTotalWidth = (count - 1) * stepX;
        const float startX = centerX - finalTotalWidth / 2.0f;
        for (int i = 0; i < count; ++i) {
            m_cards[i]->setTargetPosition(startX + i * stepX, centerY);
        }
    }
}

std::shared_ptr<Card> CardArea::getCardAt(float x, float y) {
    for (int i = static_cast<int>(m_cards.size()) - 1; i >= 0; --i) {
        if (m_cards[i]->getGlobalBounds().contains(x, y)) {
            return m_cards[i];
        }
    }
    return nullptr;
}

std::vector<Card*> CardArea::getSelectedCards() {
    std::vector<Card*> selected;
    selected.reserve(m_cards.size());
    for (auto& c : m_cards) {
        if (c->isSelected()) selected.push_back(c.get());
    }
    return selected;
}

void CardArea::update(float dt) {
    for (auto& card : m_cards) {
        card->update(dt);
    }
}

void CardArea::draw(sf::RenderTarget& target) {
    for (auto& card : m_cards) {
        card->draw(target);
    }
}
