#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

class CardView {
public:
    void init(const sf::Texture& texture, int w, int h) {
        m_base.setSize(sf::Vector2f(static_cast<float>(w), static_cast<float>(h)));
        m_base.setFillColor(sf::Color::White);
        m_base.setOutlineColor(sf::Color::Black);
        m_base.setOutlineThickness(1.0f);
        m_base.setOrigin(w / 2.0f, h / 2.0f);

        m_shadow.setSize(sf::Vector2f(static_cast<float>(w), static_cast<float>(h)));
        m_shadow.setFillColor(sf::Color(0, 0, 0, 80));
        m_shadow.setOrigin(w / 2.0f, h / 2.0f);

        m_sprite.setTexture(texture);
        m_sprite.setOrigin(w / 2.0f, h / 2.0f);

        applyTransform();
    }

    void setTextureRect(const sf::IntRect& rect) { m_sprite.setTextureRect(rect); }

    void update(bool isSelected, bool isHovered, float dt) {
        float visualOffsetY = 0.0f;
        float visualScale = m_targetScale;

        if (isSelected) {
            visualOffsetY = -30.0f;
        } else if (isHovered) {
            visualOffsetY = -10.0f;
            visualScale = m_targetScale + 0.1f;
        }

        const float moveSpeed = 15.0f;
        const float scaleSpeed = 10.0f;

        const sf::Vector2f currentPos = m_base.getPosition();
        const sf::Vector2f finalTarget(m_targetPos.x, m_targetPos.y + visualOffsetY);
        const sf::Vector2f newPos = currentPos + (finalTarget - currentPos) * moveSpeed * dt;

        m_scale = m_scale + (visualScale - m_scale) * scaleSpeed * dt;
        m_base.setPosition(newPos);
        m_sprite.setPosition(newPos);
        applyTransform();

        const float shadowOffset = 5.0f + (-visualOffsetY * 0.2f);
        m_shadow.setPosition(newPos.x + shadowOffset, newPos.y + shadowOffset);
    }

    void draw(sf::RenderTarget& target) const {
        target.draw(m_shadow);
        target.draw(m_base);
        target.draw(m_sprite);
    }

    void setTargetPosition(float x, float y) { m_targetPos = sf::Vector2f(x, y); }
    void setBaseScale(float scale) { m_targetScale = scale; }

    void setInstantPosition(float x, float y) {
        m_targetPos = sf::Vector2f(x, y);
        m_base.setPosition(x, y);
        m_sprite.setPosition(x, y);
        m_shadow.setPosition(x + 5.0f, y + 5.0f);
    }

    void setColor(const sf::Color& color) { m_sprite.setColor(color); }
    sf::FloatRect getGlobalBounds() const { return m_base.getGlobalBounds(); }
    sf::Vector2f getPosition() const { return m_base.getPosition(); }

private:
    void applyTransform() {
        m_base.setScale(m_scale, m_scale);
        m_sprite.setScale(m_scale, m_scale);
        m_shadow.setScale(m_scale, m_scale);
    }

    sf::Sprite m_sprite;
    sf::RectangleShape m_base;
    sf::RectangleShape m_shadow;

    sf::Vector2f m_targetPos;
    float m_targetScale = 3.0f;
    float m_scale = 3.0f;
};
