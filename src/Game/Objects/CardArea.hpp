#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Card.hpp"

enum class LayoutType {
    Fan, Stack, Row, Grid
};

class CardArea {
public:
    /**
     * 构造卡牌区域。
     *
     * @param x 区域左上角 X
     * @param y 区域左上角 Y
     * @param w 区域宽度
     * @param h 区域高度
     * @param type 布局类型
     */
    CardArea(float x, float y, float w, float h, LayoutType type);

    /**
     * 向区域添加卡牌。
     *
     * @param card 卡牌所有权
     */
    void addCard(std::shared_ptr<Card> card);

    /**
     * 按索引删除卡牌。
     *
     * @param index 目标索引
     */
    void removeCard(int index);

    /**
     * 取走一张卡牌并转移所有权。
     *
     * 该接口用于跨区域移动卡牌，避免“先删后建”导致状态丢失。
     *
     * @param cardPtr 目标卡牌裸指针
     * @return 取出的卡牌；未找到返回 nullptr
     */
    std::shared_ptr<Card> takeCard(Card* cardPtr);

    /**
     * 重新计算区域内卡牌布局。
     */
    void alignCards();

    /**
     * 根据坐标拾取最上层卡牌。
     *
     * @param x 世界坐标 X
     * @param y 世界坐标 Y
     * @return 命中的卡牌；未命中返回 nullptr
     */
    std::shared_ptr<Card> getCardAt(float x, float y);

    /**
     * 获取当前所有已选卡牌。
     *
     * @return 已选卡牌裸指针列表
     */
    std::vector<Card*> getSelectedCards();

    /**
     * 获取内部卡牌容器。
     *
     * @return 卡牌容器引用
     */
    std::vector<std::shared_ptr<Card>>& getCards() {
        return m_cards;
    }

    /**
     * 更新区域内卡牌动画。
     *
     * @param dt 帧间隔秒数
     */
    void update(float dt);

    /**
     * 绘制区域内卡牌。
     *
     * @param target 绘制目标
     */
    void draw(sf::RenderTarget& target);

private:
    std::vector<std::shared_ptr<Card>> m_cards;
    sf::FloatRect m_bounds;
    LayoutType m_layoutType;
    sf::RectangleShape m_debugBox;
};
