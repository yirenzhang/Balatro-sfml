#pragma once

#include <SFML/Graphics.hpp>

class InputRouter {
public:
    /**
     * 分发窗口事件。
     *
     * 统一处理关闭事件可以避免各个状态重复实现同一逻辑，
     * 让状态只关注玩法输入。
     *
     * @param window 游戏窗口
     * @param handler 业务事件处理回调
     */
    template <typename EventHandler>
    void process(sf::RenderWindow& window, EventHandler&& handler) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                continue;
            }
            handler(event);
        }
    }
};
