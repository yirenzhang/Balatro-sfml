#include "ResourceManager.hpp"

#include <iostream>

bool ResourceManager::loadTexture(const std::string& id, const std::string& filename) {
    sf::Texture tex;
    if (!tex.loadFromFile(filename)) {
        std::cerr << "[Error] Failed to load texture: " << filename << std::endl;
        return false;
    }

    tex.setSmooth(false);
    m_textures[id] = std::move(tex);
    return true;
}

sf::Texture& ResourceManager::getTexture(const std::string& id) {
    if (m_textures.find(id) == m_textures.end()) {
        std::cerr << "[Warning] Texture not found: " << id << ". Returning empty texture." << std::endl;
        static sf::Texture empty;
        return empty;
    }
    return m_textures[id];
}

bool ResourceManager::loadFont(const std::string& id, const std::string& filename) {
    sf::Font font;
    if (!font.loadFromFile(filename)) {
        std::cerr << "[Error] Failed to load font: " << filename << std::endl;
        return false;
    }

    m_fonts[id] = std::move(font);
    return true;
}

sf::Font& ResourceManager::getFont(const std::string& id) {
    if (m_fonts.find(id) == m_fonts.end()) {
        std::cerr << "[Warning] Font not found: " << id << ". Returning empty font." << std::endl;
        static sf::Font empty;
        return empty;
    }
    return m_fonts[id];
}

bool ResourceManager::loadShader(const std::string& id, const std::string& fragPath) {
    auto shader = std::make_shared<sf::Shader>();
    if (!shader->loadFromFile(fragPath, sf::Shader::Fragment)) {
        std::cerr << "[Error] Failed to load shader: " << fragPath << std::endl;
        return false;
    }

    m_shaders[id] = shader;
    return true;
}

std::shared_ptr<sf::Shader> ResourceManager::getShader(const std::string& id) {
    if (m_shaders.find(id) == m_shaders.end()) {
        return nullptr;
    }
    return m_shaders[id];
}
