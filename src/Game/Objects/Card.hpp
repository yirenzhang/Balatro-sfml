#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

#include "CardModel.hpp"
#include "CardView.hpp"

class Card {
public:
    static constexpr int DECK_WIDTH = 71;
    static constexpr int DECK_HEIGHT = 95;
    static constexpr int JOKER_WIDTH = 69;
    static constexpr int JOKER_HEIGHT = 93;
    static constexpr int JOKER_GAP = 2;
    static constexpr int JOKER_MARGIN = 1;
    static constexpr int JOKER_COLS = 10;

    Card(Suit suit, Rank rank, const sf::Texture& texture) {
        m_model.type = CardType::PlayingCard;
        m_model.suit = suit;
        m_model.rank = rank;

        m_view.init(texture, DECK_WIDTH, DECK_HEIGHT);

        const int gridX = static_cast<int>(rank) - 2;
        int gridY = 0;
        switch (suit) {
            case Suit::Hearts:   gridY = 0; break;
            case Suit::Clubs:    gridY = 1; break;
            case Suit::Diamonds: gridY = 2; break;
            case Suit::Spades:   gridY = 3; break;
            default: break;
        }
        m_view.setTextureRect(sf::IntRect(gridX * DECK_WIDTH, gridY * DECK_HEIGHT, DECK_WIDTH, DECK_HEIGHT));
    }

    Card(int jokerId, const sf::Texture& texture) {
        m_model.type = CardType::Joker;
        m_model.suit = Suit::None;
        m_model.rank = Rank::Two;

        m_view.init(texture, JOKER_WIDTH, JOKER_HEIGHT);

        const int gridX = jokerId % JOKER_COLS;
        const int gridY = jokerId / JOKER_COLS;
        const int rectX = JOKER_MARGIN + gridX * (JOKER_WIDTH + JOKER_GAP);
        const int rectY = JOKER_MARGIN + gridY * (JOKER_HEIGHT + JOKER_GAP);
        m_view.setTextureRect(sf::IntRect(rectX, rectY, JOKER_WIDTH, JOKER_HEIGHT));
    }

    void update(float dt) { m_view.update(m_model.isSelected, m_model.isHovered, dt); }
    void draw(sf::RenderTarget& target) { m_view.draw(target); }

    void setTargetPosition(float x, float y) { m_view.setTargetPosition(x, y); }
    void setBaseScale(float scale) { m_view.setBaseScale(scale); }
    void setInstantPosition(float x, float y) { m_view.setInstantPosition(x, y); }

    void setHover(bool hover) { m_model.isHovered = hover; }
    void toggleSelect() { m_model.isSelected = !m_model.isSelected; }
    void select(bool val) { m_model.isSelected = val; }
    bool isSelected() const { return m_model.isSelected; }
    sf::FloatRect getGlobalBounds() const { return m_view.getGlobalBounds(); }
    sf::Vector2f getPosition() const { return m_view.getPosition(); }
    Suit getSuit() const { return m_model.suit; }
    Rank getRank() const { return m_model.rank; }
    CardType getType() const { return m_model.type; }

    int getChips() const {
        if (m_model.type == CardType::Joker) return 0;
        return m_model.chips;
    }
    void setChips(int chips) { m_model.chips = chips; }

    void setColor(const sf::Color& color) { m_view.setColor(color); }

    void setCost(int cost) { m_model.cost = cost; }
    int getCost() const { return m_model.cost; }

    void setEffect(std::shared_ptr<IEffect> effect) { m_model.effect = std::move(effect); }
    std::shared_ptr<IEffect> getEffect() const { return m_model.effect; }
    bool isJoker() const { return m_model.effect != nullptr; }

    void setAbilityName(const std::string& name) { m_model.abilityName = name; }
    std::string getAbilityName() const { return m_model.abilityName; }

    void setDescription(const std::string& desc) { m_model.description = desc; }
    std::string getDescription() const { return m_model.description; }

    CardModel& getModel() { return m_model; }
    const CardModel& getModel() const { return m_model; }

private:
    CardModel m_model;
    CardView m_view;
};
