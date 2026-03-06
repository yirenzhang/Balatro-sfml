#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Tooltip {
public:
    void init(const sf::Font& font);
    void update(const std::string& name, const std::string& desc, sf::Vector2f mousePos);
    void draw(sf::RenderTarget& target);
private:
    sf::RectangleShape m_background;
    sf::Text m_textName;
    sf::Text m_textDesc;
};
