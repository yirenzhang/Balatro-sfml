#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm> 
#include "Card.hpp"

enum class LayoutType {
    Fan, Stack, Row, Grid
};

class CardArea {
public:
    CardArea(float x, float y, float w, float h, LayoutType type)
        : m_bounds(x, y, w, h), m_layoutType(type) {
        
        m_debugBox.setPosition(x, y);
        m_debugBox.setSize(sf::Vector2f(w, h));
        m_debugBox.setFillColor(sf::Color::Transparent);
        m_debugBox.setOutlineColor(sf::Color(255, 255, 255, 50));
        m_debugBox.setOutlineThickness(1.0f);
    }

    void addCard(std::shared_ptr<Card> card) {
        m_cards.push_back(card);
    }

    // 按索引删除 (不需要返回值，直接销毁)
    void removeCard(int index) {
        if (index >= 0 && index < (int)m_cards.size()) {
            m_cards.erase(m_cards.begin() + index);
            alignCards();
        }
    }

    // [删除旧的 bool removeCard(Card*)] 
    // [New] 拿走卡牌 (转移所有权，防止对象析构)
    std::shared_ptr<Card> takeCard(Card* cardPtr) {
        // 使用 find_if 找到对应的智能指针
        auto it = std::find_if(m_cards.begin(), m_cards.end(), 
            [cardPtr](const std::shared_ptr<Card>& ptr) {
                return ptr.get() == cardPtr;
            });

        if (it != m_cards.end()) {
            std::shared_ptr<Card> card = *it; // 1. 先持有它，引用计数+1
            m_cards.erase(it);                // 2. 从列表移除，引用计数-1（但因为我们在上面持有了，所以不会归零）
            alignCards();
            return card;                      // 3. 返回活着的指针
        }
        return nullptr;
    }

    // ... (alignCards, getCardAt 等其他方法保持不变) ...
    // 这里为了节省篇幅省略了 alignCards 实现，请保留你原来的代码

    void alignCards() {
        if (m_cards.empty()) return;
        float cardW = Card::DECK_WIDTH * 3.0f; 
        if (!m_cards.empty()) {
            cardW = m_cards[0]->getGlobalBounds().width;
            if (cardW < 10.0f) cardW = Card::DECK_WIDTH * 3.0f;
        }
        
        float centerX = m_bounds.left + m_bounds.width / 2.0f;
        float centerY = m_bounds.top + m_bounds.height / 2.0f;
        int count = (int)m_cards.size();

        float maxSpacing = cardW * 1.1f; 
        if (m_layoutType == LayoutType::Fan) maxSpacing = cardW * 0.5f; 

        float currentTotalWidth = (count - 1) * maxSpacing + cardW;
        float availableWidth = m_bounds.width - 20.0f; 

        float stepX = maxSpacing;
        if (currentTotalWidth > availableWidth && count > 1) {
            stepX = (availableWidth - cardW) / (float)(count - 1);
        }

        if (m_layoutType == LayoutType::Fan) {
            float finalTotalWidth = (count - 1) * stepX;
            float startX = centerX - finalTotalWidth / 2.0f;
            for (int i = 0; i < count; ++i) {
                float x = startX + i * stepX;
                float y = centerY;
                float relativeX = (i - (count - 1) / 2.0f); 
                float yOffset = std::abs(relativeX) * std::abs(relativeX) * 2.0f; 
                m_cards[i]->setTargetPosition(x, y + yOffset + 20.0f); 
            }
        }
        else if (m_layoutType == LayoutType::Row) {
            float finalTotalWidth = (count - 1) * stepX;
            float startX = centerX - finalTotalWidth / 2.0f;
            for(int i=0; i<count; ++i) {
                m_cards[i]->setTargetPosition(startX + i * stepX, centerY);
            }
        }
    }

    std::shared_ptr<Card> getCardAt(float x, float y) {
        for (int i = (int)m_cards.size() - 1; i >= 0; --i) {
            if (m_cards[i]->getGlobalBounds().contains(x, y)) {
                return m_cards[i];
            }
        }
        return nullptr;
    }

    std::vector<Card*> getSelectedCards() {
        std::vector<Card*> selected;
        for (auto& c : m_cards) {
            if (c->isSelected()) selected.push_back(c.get());
        }
        return selected;
    }

    std::vector<std::shared_ptr<Card>>& getCards() {
        return m_cards;
    }

    void update(float dt) {
        for (auto& card : m_cards) {
            card->update(dt);
        }
    }

    void draw(sf::RenderTarget& target) {
        for (auto& card : m_cards) {
            card->draw(target);
        }
    }

private:
    std::vector<std::shared_ptr<Card>> m_cards;
    sf::FloatRect m_bounds;
    LayoutType m_layoutType;
    sf::RectangleShape m_debugBox;
};