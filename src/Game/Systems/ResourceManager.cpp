#include "ResourceManager.hpp"

#include <iostream>

void ResourceManager::recordError(const std::string& msg) {
    m_errors.push_back(msg);
    std::cerr << msg << std::endl;
}

bool ResourceManager::loadTexture(const std::string& id, const std::string& filename) {
    sf::Texture tex;
    if (!tex.loadFromFile(filename)) {
        recordError("[Error] Failed to load texture: " + filename + " (id=" + id + ")");
        return false;
    }

    tex.setSmooth(false);
    m_textures[id] = std::move(tex);
    return true;
}

sf::Texture& ResourceManager::getTexture(const std::string& id) {
    if (m_textures.find(id) == m_textures.end()) {
        recordError("[Error] Texture not found: " + id + ". Returning empty texture.");
        static sf::Texture empty;
        return empty;
    }
    return m_textures[id];
}

bool ResourceManager::loadFont(const std::string& id, const std::string& filename) {
    sf::Font font;
    if (!font.loadFromFile(filename)) {
        recordError("[Error] Failed to load font: " + filename + " (id=" + id + ")");
        return false;
    }

    m_fonts[id] = std::move(font);
    return true;
}

sf::Font& ResourceManager::getFont(const std::string& id) {
    if (m_fonts.find(id) == m_fonts.end()) {
        recordError("[Error] Font not found: " + id + ". Returning empty font.");
        static sf::Font empty;
        return empty;
    }
    return m_fonts[id];
}

bool ResourceManager::loadShader(const std::string& id, const std::string& fragPath) {
    auto shader = std::make_shared<sf::Shader>();
    if (!shader->loadFromFile(fragPath, sf::Shader::Fragment)) {
        recordError("[Error] Failed to load shader: " + fragPath + " (id=" + id + ")");
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

bool ResourceManager::hasTexture(const std::string& id) const {
    return m_textures.find(id) != m_textures.end();
}

bool ResourceManager::hasFont(const std::string& id) const {
    return m_fonts.find(id) != m_fonts.end();
}

bool ResourceManager::hasShader(const std::string& id) const {
    return m_shaders.find(id) != m_shaders.end();
}
