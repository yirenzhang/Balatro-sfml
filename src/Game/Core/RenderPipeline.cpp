#include "RenderPipeline.hpp"

bool RenderPipeline::init(unsigned width, unsigned height) {
    return m_renderTexture.create(width, height);
}

bool RenderPipeline::loadShader(const std::string& fragPath) {
    if (!m_crtShader.loadFromFile(fragPath, sf::Shader::Fragment)) {
        m_shaderLoaded = false;
        return false;
    }

    m_shaderLoaded = true;
    m_crtShader.setUniform("texture", sf::Shader::CurrentTexture);
    m_crtShader.setUniform(
        "resolution",
        sf::Vector2f(
            static_cast<float>(m_renderTexture.getSize().x),
            static_cast<float>(m_renderTexture.getSize().y)
        )
    );
    return true;
}

void RenderPipeline::update(float dt) {
    if (!m_shaderLoaded) return;

    m_shaderTime += dt;
    if (m_shaderTime > 1000.0f) {
        m_shaderTime = 0.0f;
    }
}

void RenderPipeline::beginFrame(const sf::Color& clearColor) {
    m_renderTexture.clear(clearColor);
}

void RenderPipeline::endFrame() {
    m_renderTexture.display();
}

void RenderPipeline::present(sf::RenderWindow& window, const CRTParams& params) {
    window.clear();
    sf::Sprite canvas(m_renderTexture.getTexture());

    if (m_shaderLoaded) {
        m_crtShader.setUniform("texture", sf::Shader::CurrentTexture);
        m_crtShader.setUniform("time", m_shaderTime);
        m_crtShader.setUniform(
            "resolution",
            sf::Vector2f(
                static_cast<float>(m_renderTexture.getSize().x),
                static_cast<float>(m_renderTexture.getSize().y)
            )
        );
        params.applyTo(m_crtShader);
        window.draw(canvas, &m_crtShader);
    } else {
        window.draw(canvas);
    }

    window.display();
}
