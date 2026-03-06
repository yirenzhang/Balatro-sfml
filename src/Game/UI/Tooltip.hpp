#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Tooltip {
public:
    /**
     * 初始化提示框样式。
     *
     * @param font 提示框字体
     */
    void init(const sf::Font& font);

    /**
     * 更新提示框内容与位置。
     *
     * @param name 标题
     * @param desc 描述
     * @param mousePos 鼠标位置
     */
    void update(const std::string& name, const std::string& desc, sf::Vector2f mousePos);

    /**
     * 绘制提示框。
     *
     * @param target 绘制目标
     */
    void draw(sf::RenderTarget& target);
private:
    sf::RectangleShape m_background;
    sf::Text m_textName;
    sf::Text m_textDesc;
};
