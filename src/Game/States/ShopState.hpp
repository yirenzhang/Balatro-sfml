#pragma once
#include "IGameState.hpp"
#include "../Core/Game.hpp"
#include "../Systems/GameDatabase.hpp"
#include <cmath>

class ShopState : public IGameState {
public:
    void onEnter(Game& game) override {
        game.getContext().state = GameState::Shop;
        std::cout << ">>> Enter Shop State <<<" << std::endl;
        // 进入商店时自动进货
        restockShop(game);
        game.getUI().setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
    }

    void onExit(Game& game) override {
        m_pendingPurchase = nullptr; // 清理选中状态
    }

    // [关键] 此函数包含切换回 RunState 的逻辑，实现在 StateDefinitions.hpp 中
    void handleEvent(Game& game, const sf::Event& event) override;

    void update(Game& game, float dt) override {
        GameContext& ctx = game.getContext();
        
        if (ctx.area_shop) ctx.area_shop->update(dt);
        
        // 选中商品时的呼吸灯效果
        if (m_pendingPurchase && ctx.area_shop) {
             static float time = 0.0f;
             time += dt;
             float flash = (std::sin(time * 10.0f) + 1.0f) * 0.5f; 
             sf::Uint8 gb = 100 + (sf::Uint8)(flash * 155); 
             m_pendingPurchase->setColor(sf::Color(255, gb, gb)); 
             
             // 重置其他卡片颜色
             for (auto& card : ctx.area_shop->getCards()) {
                if (card.get() != m_pendingPurchase) card->setColor(sf::Color::White);
             }
        }
    }

    void draw(Game& game, sf::RenderTarget& target) override {
        GameContext& ctx = game.getContext();
        if (ctx.area_shop) ctx.area_shop->draw(target);
        
        // 让 UI Manager 绘制商店特定的 UI (提示文字)
        game.getUI().draw(target, GameState::Shop);
    }

private:
    Card* m_pendingPurchase = nullptr;

    // 进货逻辑 (ShopState 内部使用)
    void restockShop(Game& game) {
        GameContext& ctx = game.getContext();
        if (!ctx.area_shop) return;
        
        while (!ctx.area_shop->getCards().empty()) ctx.area_shop->removeCard(0);

        std::vector<std::string> jokerPool = {"j_joker", "j_greedy_joker", "j_abstract"};
        for (int i = 0; i < 3; ++i) {
            std::string id = jokerPool[std::rand() % jokerPool.size()];
            // 注意：GameDatabase 现在不需要传 texture 了
            if (auto card = GameDatabase::Instance().createJoker(id)) {
                ctx.area_shop->addCard(card);
            }
        }
        ctx.area_shop->alignCards();
    }
};