#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

class FloatingText {
public:
    /**
     * 构造飘字对象。
     *
     * 飘字用于强化结算反馈，帮助玩家在短时间内建立“动作-结果”关联。
     *
     * @param str 文本内容
     * @param pos 初始位置
     * @param color 文本颜色
     * @param font 使用字体
     */
    FloatingText(const std::string& str, sf::Vector2f pos, sf::Color color, const sf::Font& font) {
        m_text.setFont(font);
        m_text.setString(str);
        m_text.setCharacterSize(40);
        m_text.setFillColor(color);

        m_text.setOutlineColor(sf::Color::Black);
        m_text.setOutlineThickness(1.5f);

        const sf::FloatRect bounds = m_text.getLocalBounds();
        m_text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        m_text.setPosition(pos);

        m_velocity = sf::Vector2f(0.0f, -80.0f);
    }

    /**
     * 更新飘字动画。
     *
     * 先位移再淡出，可让反馈优先被注意到，再自然退出视觉焦点。
     *
     * @param dt 帧间隔秒数
     */
    void update(float dt) {
        m_lifetime += dt;
        m_text.move(m_velocity * dt);
        m_velocity.y *= 0.98f;

        const float fadeDuration = 0.5f;
        const float fadeStart = m_maxLifetime - fadeDuration;
        if (m_lifetime > fadeStart) {
            float alpha = 255.0f * (1.0f - (m_lifetime - fadeStart) / fadeDuration);
            if (alpha < 0) alpha = 0;

            const sf::Color c = m_text.getFillColor();
            m_text.setFillColor(sf::Color(c.r, c.g, c.b, static_cast<sf::Uint8>(alpha)));

            const sf::Color oc = m_text.getOutlineColor();
            m_text.setOutlineColor(sf::Color(oc.r, oc.g, oc.b, static_cast<sf::Uint8>(alpha)));
        }
    }

    /**
     * 绘制飘字。
     *
     * @param target 绘制目标
     */
    void draw(sf::RenderTarget& target) {
        target.draw(m_text);
    }

    /**
     * 判断飘字是否结束。
     *
     * @return 若生命周期结束返回 true
     */
    bool isDead() const {
        return m_lifetime >= m_maxLifetime;
    }

private:
    sf::Text m_text;
    sf::Vector2f m_velocity;
    float m_lifetime = 0.0f;
    float m_maxLifetime = 2.0f;
};
