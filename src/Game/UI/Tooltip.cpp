#include "Tooltip.hpp"

#include <algorithm>

void Tooltip::init(const sf::Font& font) {
    m_textName.setFont(font);
    m_textName.setCharacterSize(24);
    m_textName.setFillColor(sf::Color::White);
    m_textName.setStyle(sf::Text::Bold);

    m_textDesc.setFont(font);
    m_textDesc.setCharacterSize(18);
    m_textDesc.setFillColor(sf::Color(200, 200, 200));

    m_background.setFillColor(sf::Color(30, 30, 30, 230));
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(2.0f);
}

void Tooltip::update(const std::string& name, const std::string& desc, sf::Vector2f mousePos) {
    m_textName.setString(name);
    m_textDesc.setString(desc);

    float width = std::max(m_textName.getGlobalBounds().width, m_textDesc.getGlobalBounds().width) + 20.0f;
    float height = m_textName.getGlobalBounds().height + m_textDesc.getGlobalBounds().height + 30.0f;
    if (width < 150.0f) {
        width = 150.0f;
    }

    m_background.setSize(sf::Vector2f(width, height));

    float offsetX = 15.0f;
    float offsetY = 15.0f;
    if (mousePos.x + width + offsetX > 1280.0f) {
        offsetX = -width - 10.0f;
    }

    const float finalX = mousePos.x + offsetX;
    const float finalY = mousePos.y + offsetY;

    m_background.setPosition(finalX, finalY);
    m_textName.setPosition(finalX + 10.0f, finalY + 10.0f);
    m_textDesc.setPosition(finalX + 10.0f, finalY + 10.0f + m_textName.getGlobalBounds().height + 10.0f);
}

void Tooltip::draw(sf::RenderTarget& target) {
    target.draw(m_background);
    target.draw(m_textName);
    target.draw(m_textDesc);
}
