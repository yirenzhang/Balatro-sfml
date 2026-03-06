#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Card.hpp"

enum class LayoutType {
    Fan, Stack, Row, Grid
};

class CardArea {
public:
    CardArea(float x, float y, float w, float h, LayoutType type);

    void addCard(std::shared_ptr<Card> card);

    // 按索引删除 (不需要返回值，直接销毁)
    void removeCard(int index);

    // [删除旧的 bool removeCard(Card*)] 
    // [New] 拿走卡牌 (转移所有权，防止对象析构)
    std::shared_ptr<Card> takeCard(Card* cardPtr);

    void alignCards();
    std::shared_ptr<Card> getCardAt(float x, float y);
    std::vector<Card*> getSelectedCards();

    std::vector<std::shared_ptr<Card>>& getCards() {
        return m_cards;
    }

    void update(float dt);

    void draw(sf::RenderTarget& target);

private:
    std::vector<std::shared_ptr<Card>> m_cards;
    sf::FloatRect m_bounds;
    LayoutType m_layoutType;
    sf::RectangleShape m_debugBox;
};
