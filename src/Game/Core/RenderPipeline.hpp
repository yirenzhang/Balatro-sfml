#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "../Data/CRTParams.hpp"

class RenderPipeline {
public:
    bool init(unsigned width, unsigned height);
    bool loadShader(const std::string& fragPath);

    void update(float dt);
    void beginFrame(const sf::Color& clearColor);
    void endFrame();

    sf::RenderTarget& target() { return m_renderTexture; }
    void present(sf::RenderWindow& window, const CRTParams& params);

private:
    sf::RenderTexture m_renderTexture;
    sf::Shader m_crtShader;
    bool m_shaderLoaded = false;
    float m_shaderTime = 0.0f;
};
