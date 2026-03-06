#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <vector>

#include "../Data/JokerData.hpp"
#include "../Objects/Card.hpp"

class ResourceManager;

/**
 * 游戏静态数据仓库与对象工厂。
 */
class GameDatabase {
public:
    /**
     * 构造数据库对象。
     */
    GameDatabase();

    /**
     * 加载 Joker 配置。
     *
     * @param filepath 配置文件路径
     * @return 加载是否成功
     */
    bool loadJokers(const std::string& filepath);

    /**
     * 注入资源管理器。
     *
     * 工厂创建 Joker 时依赖纹理，因此通过注入而非全局访问。
     *
     * @param resourceManager 资源管理器指针
     */
    void setResourceManager(ResourceManager* resourceManager);

    /**
     * 创建 Joker 卡牌实例。
     *
     * @param jokerId Joker 配置 ID
     * @return Joker 卡牌；创建失败返回 nullptr
     */
    std::shared_ptr<Card> createJoker(const std::string& jokerId);

    /**
     * 加载点数筹码配置。
     *
     * @param filepath 配置文件路径
     * @return 加载是否成功
     */
    bool loadRanks(const std::string& filepath);

    /**
     * 查询点数基础筹码。
     *
     * @param rank 点数
     * @return 对应筹码值
     */
    int getRankChips(Rank rank) const;

    /**
     * 获取所有 Joker ID。
     *
     * @return Joker ID 列表
     */
    std::vector<std::string> getAllJokerIds() const;

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
    void recordError(const std::string& msg);

    std::unordered_map<std::string, JokerData> m_jokerDb;
    std::map<int, int> m_rankChips; 
    ResourceManager* m_resourceManager = nullptr;
    std::vector<std::string> m_errors;
};
