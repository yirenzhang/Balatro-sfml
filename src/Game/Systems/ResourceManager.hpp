#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>
#include <vector>

class ResourceManager {
public:
    ResourceManager() = default;

    bool loadTexture(const std::string& id, const std::string& filename);
    sf::Texture& getTexture(const std::string& id);

    bool loadFont(const std::string& id, const std::string& filename);
    sf::Font& getFont(const std::string& id);

    bool loadShader(const std::string& id, const std::string& fragPath);
    std::shared_ptr<sf::Shader> getShader(const std::string& id);

    bool hasTexture(const std::string& id) const;
    bool hasFont(const std::string& id) const;
    bool hasShader(const std::string& id) const;

    const std::vector<std::string>& errors() const { return m_errors; }
    void clearErrors() { m_errors.clear(); }

private:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void recordError(const std::string& msg);

    std::map<std::string, sf::Texture> m_textures;
    std::map<std::string, sf::Font> m_fonts;
    std::map<std::string, std::shared_ptr<sf::Shader>> m_shaders;
    std::vector<std::string> m_errors;
};
