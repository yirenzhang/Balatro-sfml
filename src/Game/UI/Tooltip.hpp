#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Tooltip {
public:
    void init(const sf::Font& font) {
        // 1. 初始化文字
        m_textName.setFont(font);
        m_textName.setCharacterSize(24);
        m_textName.setFillColor(sf::Color::White);
        m_textName.setStyle(sf::Text::Bold);

        m_textDesc.setFont(font);
        m_textDesc.setCharacterSize(18);
        m_textDesc.setFillColor(sf::Color(200, 200, 200)); // 浅灰色

        // 2. 初始化背景框
        m_background.setFillColor(sf::Color(30, 30, 30, 230)); // 深灰半透明
        m_background.setOutlineColor(sf::Color::White);
        m_background.setOutlineThickness(2.0f);
    }

    void update(const std::string& name, const std::string& desc, sf::Vector2f mousePos) {
        // 设置内容
        m_textName.setString(name);
        m_textDesc.setString(desc);

        // 简单的排版：名字在上，描述在下
        // 设置文字换行宽度 (这里简化处理，手动换行或不做处理)
        // 实际项目中需要写一个 Text Wrapper 算法
        
        // 计算尺寸
        float width = std::max(m_textName.getGlobalBounds().width, m_textDesc.getGlobalBounds().width) + 20.0f;
        float height = m_textName.getGlobalBounds().height + m_textDesc.getGlobalBounds().height + 30.0f;
        
        // 最小宽度
        if (width < 150.0f) width = 150.0f;

        m_background.setSize(sf::Vector2f(width, height));

        // 跟随鼠标位置 (稍微偏移一点，避免遮挡光标)
        float offsetX = 15.0f;
        float offsetY = 15.0f;
        
        // 防止超出屏幕右边界 (简单逻辑)
        if (mousePos.x + width + offsetX > 1280.0f) {
            offsetX = -width - 10.0f; // 改为显示在左边
        }

        float finalX = mousePos.x + offsetX;
        float finalY = mousePos.y + offsetY;

        m_background.setPosition(finalX, finalY);
        
        m_textName.setPosition(finalX + 10.0f, finalY + 10.0f);
        // 描述文字在名字下方
        m_textDesc.setPosition(finalX + 10.0f, finalY + 10.0f + m_textName.getGlobalBounds().height + 10.0f);
    }

    void draw(sf::RenderTarget& target) {
        target.draw(m_background);
        target.draw(m_textName);
        target.draw(m_textDesc);
    }
private:
    sf::RectangleShape m_background;
    sf::Text m_textName;
    sf::Text m_textDesc;
};