#pragma once

#include <SFML/Graphics.hpp>

class InputRouter {
public:
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
