#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <iostream>
#include <memory>

class ResourceManager {
public:
    // 单例访问点
    static ResourceManager& Instance() {
        static ResourceManager instance;
        return instance;
    }

    // --- 纹理管理 ---
    bool loadTexture(const std::string& id, const std::string& filename) {
        sf::Texture tex;
        if (!tex.loadFromFile(filename)) {
            std::cerr << "[Error] Failed to load texture: " << filename << std::endl;
            return false;
        }
        // 对于像素风游戏，通常不需要平滑
        tex.setSmooth(false); 
        m_textures[id] = std::move(tex);
        return true;
    }

    sf::Texture& getTexture(const std::string& id) {
        if (m_textures.find(id) == m_textures.end()) {
            std::cerr << "[Warning] Texture not found: " << id << ". Returning empty texture." << std::endl;
            static sf::Texture empty;
            return empty;
        }
        return m_textures[id];
    }

    // --- 字体管理 ---
    bool loadFont(const std::string& id, const std::string& filename) {
        sf::Font font;
        if (!font.loadFromFile(filename)) {
            std::cerr << "[Error] Failed to load font: " << filename << std::endl;
            return false;
        }
        m_fonts[id] = std::move(font);
        return true;
    }

    sf::Font& getFont(const std::string& id) {
        if (m_fonts.find(id) == m_fonts.end()) {
            std::cerr << "[Warning] Font not found: " << id << ". Returning empty font." << std::endl;
            static sf::Font empty;
            return empty;
        }
        return m_fonts[id];
    }

    // --- Shader 管理 (可选) ---
    // Balatro 的 Shader 通常只需要 Fragment Shader
    bool loadShader(const std::string& id, const std::string& fragPath) {
        auto shader = std::make_shared<sf::Shader>();
        if (!shader->loadFromFile(fragPath, sf::Shader::Fragment)) {
            std::cerr << "[Error] Failed to load shader: " << fragPath << std::endl;
            return false;
        }
        m_shaders[id] = shader;
        return true;
    }

    std::shared_ptr<sf::Shader> getShader(const std::string& id) {
        if (m_shaders.find(id) == m_shaders.end()) {
            return nullptr;
        }
        return m_shaders[id];
    }

private:
    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, sf::Texture> m_textures;
    std::map<std::string, sf::Font> m_fonts;
    std::map<std::string, std::shared_ptr<sf::Shader>> m_shaders;
};