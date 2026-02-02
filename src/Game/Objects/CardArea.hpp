#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm> // [新增] for std::remove_if
#include "Card.hpp"

enum class LayoutType {
    Fan,    // 扇形展开 (手牌)
    Stack,  // 堆叠 (牌堆)
    Row,    // 横向排列 (弃牌区/商店/Joker)
    Grid    // 网格
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

    // 按索引删除
    void removeCard(int index) {
        if (index >= 0 && index < (int)m_cards.size()) {
            m_cards.erase(m_cards.begin() + index);
            alignCards();
        }
    }

    // [新增] 按指针删除 (用于购买逻辑)
    // 如果找到了并删除了返回 true，否则返回 false
    bool removeCard(Card* cardPtr) {
        auto it = std::remove_if(m_cards.begin(), m_cards.end(), 
            [cardPtr](const std::shared_ptr<Card>& ptr) {
                return ptr.get() == cardPtr;
            });

        if (it != m_cards.end()) {
            m_cards.erase(it, m_cards.end());
            alignCards();
            return true;
        }
        return false;
    }

    // 核心功能：排列卡牌
    void alignCards() {
        if (m_cards.empty()) return;

        // [修复] 不再调用 getScale()，而是直接获取包围盒宽度
        // 默认宽度 (万一没有卡牌或者出错)
        float cardW = Card::DECK_WIDTH * 3.0f; 
        
        // 如果有卡牌，直接使用第一张牌的实际显示宽度 (包含了缩放)
        if (!m_cards.empty()) {
            cardW = m_cards[0]->getGlobalBounds().width;
            
            // 安全检查：如果获取到的宽度太小(比如还没初始化好)，就用回默认值
            if (cardW < 10.0f) cardW = Card::DECK_WIDTH * 3.0f;
        }
        
        float centerX = m_bounds.left + m_bounds.width / 2.0f;
        float centerY = m_bounds.top + m_bounds.height / 2.0f;
        int count = (int)m_cards.size();

        // --- 动态间距计算 (Squeeze Logic) ---
        float maxSpacing = cardW * 1.1f; // Joker 默认宽一点
        if (m_layoutType == LayoutType::Fan) maxSpacing = cardW * 0.5f; // 手牌紧凑一点

        float currentTotalWidth = (count - 1) * maxSpacing + cardW;
        float availableWidth = m_bounds.width - 20.0f; 

        float stepX = maxSpacing;
        if (currentTotalWidth > availableWidth && count > 1) {
            stepX = (availableWidth - cardW) / (float)(count - 1);
        }

        // --- 排版 ---
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
        else if (m_layoutType == LayoutType::Stack) {
            for (auto& card : m_cards) {
                card->setTargetPosition(centerX, centerY);
            }
        }
    }

    // 获取鼠标位置下的最上层卡牌
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
        // target.draw(m_debugBox); // 调试框
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