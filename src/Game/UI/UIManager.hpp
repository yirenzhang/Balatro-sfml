#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "../Core/GameContext.hpp"

class UIManager {
public:
    UIManager() = default;

    void init(const sf::Font& font);
    void update(const GameContext& ctx);
    void updateHandInfo(const std::string& handName, int level, int chips, int mult);
    void setShopMessage(const std::string& msg, sf::Color color);
    void draw(sf::RenderTarget& target, GameState state);

private:
    void setupText(sf::Text& t, int size, sf::Color color, sf::Vector2f pos, const sf::Font& font);
    void centerTextInBox(sf::Text& t, const sf::RectangleShape& box);

    sf::Text m_textHUD;
    sf::Text m_textScore;
    sf::Text m_textDeckCount;
    sf::Text m_textShopInfo;

    sf::RectangleShape m_handInfoBg;
    sf::RectangleShape m_chipsBox;
    sf::RectangleShape m_multBox;
    sf::Text m_textHandType;
    sf::Text m_textHandLevel;
    sf::Text m_textBaseChips;
    sf::Text m_textBaseMult;
    sf::Text m_textMultSymbol;
};
