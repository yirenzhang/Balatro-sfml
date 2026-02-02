#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>

class FloatingText {
public:
    // 构造函数：传入内容、位置、颜色、字体
    FloatingText(const std::string& str, sf::Vector2f pos, sf::Color color, const sf::Font& font) {
        m_text.setFont(font);
        m_text.setString(str);
        m_text.setCharacterSize(40); // 字体大小
        m_text.setFillColor(color);
        
        // 加个黑色描边，防止在复杂背景上看不清
        m_text.setOutlineColor(sf::Color::Black);
        m_text.setOutlineThickness(1.5f);

        // 设置原点为中心，方便定位
        sf::FloatRect bounds = m_text.getLocalBounds();
        m_text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        m_text.setPosition(pos);

        // 初始速度：向上飘 (Y轴负方向)
        m_velocity = sf::Vector2f(0.0f, -80.0f); 
    }

    // 更新动画
    void update(float dt) {
        m_lifetime += dt;

        // 1. 移动
        m_text.move(m_velocity * dt);
        
        // 2. 减速 (可选，模拟一点重力感，或者保持匀速)
        // m_velocity.y += 50.0f * dt; 
        m_velocity.y *= 0.98f;


        // 3. 淡出效果，假设总寿命 2.0s，我想让它最后 0.5s 才开始淡出
        float fadeDuration = 0.5f;
        float fadeStart = m_maxLifetime - fadeDuration;
        
        if (m_lifetime > fadeStart) {
            float alpha = 255.0f * (1.0f - (m_lifetime - fadeStart) / fadeDuration);
            if (alpha < 0) alpha = 0;
            
            sf::Color c = m_text.getFillColor();
            m_text.setFillColor(sf::Color(c.r, c.g, c.b, (sf::Uint8)alpha));
            
            sf::Color oc = m_text.getOutlineColor();
            m_text.setOutlineColor(sf::Color(oc.r, oc.g, oc.b, (sf::Uint8)alpha));
        }
    }

    // 绘制
    void draw(sf::RenderTarget& target) {
        target.draw(m_text);
    }

    // 是否存活
    bool isDead() const {
        return m_lifetime >= m_maxLifetime;
    }

private:
    sf::Text m_text;
    sf::Vector2f m_velocity;
    float m_lifetime = 0.0f;
    float m_maxLifetime = 2.0f; // 存活 1 秒
};