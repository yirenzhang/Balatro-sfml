#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>
#include <vector>

class ResourceManager {
public:
    /**
     * 构造资源管理器。
     */
    ResourceManager() = default;

    /**
     * 加载纹理资源。
     *
     * @param id 资源 ID
     * @param filename 文件路径
     * @return 加载是否成功
     */
    bool loadTexture(const std::string& id, const std::string& filename);

    /**
     * 获取纹理资源。
     *
     * @param id 资源 ID
     * @return 纹理引用；缺失时返回空纹理占位
     */
    sf::Texture& getTexture(const std::string& id);

    /**
     * 加载字体资源。
     *
     * @param id 资源 ID
     * @param filename 文件路径
     * @return 加载是否成功
     */
    bool loadFont(const std::string& id, const std::string& filename);

    /**
     * 获取字体资源。
     *
     * @param id 资源 ID
     * @return 字体引用；缺失时返回空字体占位
     */
    sf::Font& getFont(const std::string& id);

    /**
     * 加载片元 shader。
     *
     * @param id 资源 ID
     * @param fragPath shader 路径
     * @return 加载是否成功
     */
    bool loadShader(const std::string& id, const std::string& fragPath);

    /**
     * 获取 shader 资源。
     *
     * @param id 资源 ID
     * @return shader 共享指针；缺失时返回 nullptr
     */
    std::shared_ptr<sf::Shader> getShader(const std::string& id);

    /**
     * 检查纹理是否已加载。
     *
     * @param id 资源 ID
     * @return 是否已存在
     */
    bool hasTexture(const std::string& id) const;

    /**
     * 检查字体是否已加载。
     *
     * @param id 资源 ID
     * @return 是否已存在
     */
    bool hasFont(const std::string& id) const;

    /**
     * 检查 shader 是否已加载。
     *
     * @param id 资源 ID
     * @return 是否已存在
     */
    bool hasShader(const std::string& id) const;

    /**
     * 获取错误日志。
     *
     * @return 错误消息列表
     */
    const std::vector<std::string>& errors() const { return m_errors; }

    /**
     * 清空错误日志。
     */
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
