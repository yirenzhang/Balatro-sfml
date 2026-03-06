#include "ShopState.hpp"
#include "RunState.hpp"
#include "../Core/Game.hpp"
#include "../Systems/GameDatabase.hpp"
#include "../Systems/ShopFlow.hpp"
#include "../Systems/ShopRestock.hpp"
#include <cmath>
#include <iostream>
#include <vector>

void ShopState::onEnter(Game& game) {
    game.getContext().state = GameState::Shop;
    std::cout << ">>> Enter Shop State <<<" << std::endl;
    
    // 入场即补货，确保商店状态可立即交互。
    restockShop(game);
    game.getUI().setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
}

void ShopState::onExit([[maybe_unused]] Game& game) {
    // 离场清理待替换引用，避免下一次进入时误用旧指针。
    m_pendingPurchase.reset();
}

void ShopState::handleEvent(Game& game, const sf::Event& event) {
    GameContext& ctx = game.getContext();
    UIManager& ui = game.getUI();

    if (event.type == sf::Event::KeyPressed) {
        // 下一轮切换入口。
        if (event.key.code == sf::Keyboard::N) {
            game.changeState(std::make_unique<RunState>());
            return;
        }
        
        // 刷新分支统一通过 ShopFlow 扣费，保持规则一致。
        if (event.key.code == sf::Keyboard::R) {
            if (ShopFlow::TryReroll(ctx)) {
                restockShop(game);
                m_pendingPurchase.reset();
            }
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
        
        // 点击 Joker 区通常表示“将待购牌替换到该槽位”。
        auto clickedJoker = ctx.jokerArea().getCardAt(mousePos.x, mousePos.y);

        // 先尝试锁定待替换牌，避免 weak_ptr 失效后误操作。
        auto pendingCard = m_pendingPurchase.lock();

        if (pendingCard && clickedJoker) {
            int cost = pendingCard->getCost();
            if (ShopFlow::TrySpend(ctx, cost)) {
                
                ctx.jokerArea().takeCard(clickedJoker.get());

                if (auto newCard = ctx.shopArea().takeCard(pendingCard.get())) {
                    newCard->setColor(sf::Color::White);
                    ctx.jokerArea().addCard(newCard);
                }

                m_pendingPurchase.reset();
                ui.setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
                
                ctx.jokerArea().alignCards();
                ctx.shopArea().alignCards();
            }
            return;
        }

        // 点击商店区是购买入口。
        auto clickedShopCard = ctx.shopArea().getCardAt(mousePos.x, mousePos.y);
        if (clickedShopCard) {
            int cost = clickedShopCard->getCost();
            const auto decision = ShopFlow::EvaluatePurchase(ctx, cost);
            if (!decision.affordable) return;
            
            // 有空槽时直接购入，减少不必要的替换步骤。
            if (!decision.requiresReplace) {
                if (!ShopFlow::TrySpend(ctx, cost)) return;
                
                if (auto newCard = ctx.shopArea().takeCard(clickedShopCard.get())) {
                    newCard->setColor(sf::Color::White);
                    ctx.jokerArea().addCard(newCard);
                }
                
                // 清理旧高亮，避免多张候选牌同时高亮造成误导。
                if (auto oldPending = m_pendingPurchase.lock()) {
                    oldPending->setColor(sf::Color::White);
                }
                m_pendingPurchase.reset();

                ui.setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
                ctx.jokerArea().alignCards();
                ctx.shopArea().alignCards();
            }
            // 无空槽时进入“先选商品，再选替换目标”流程。
            else {
                if (auto oldPending = m_pendingPurchase.lock()) {
                    oldPending->setColor(sf::Color::White);
                }
                
                m_pendingPurchase = clickedShopCard; 
                
                ui.setShopMessage("SELECT A JOKER TO REPLACE\n[Right Click] Cancel", sf::Color::Red);
            }
        }
    }

    // 右键随时取消替换流程，降低误操作成本。
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
        if (auto pendingCard = m_pendingPurchase.lock()) {
            pendingCard->setColor(sf::Color::White);
        }
        m_pendingPurchase.reset();
        ui.setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
    }
}

void ShopState::update(Game& game, float dt) {
    GameContext& ctx = game.getContext();
    
    if (ctx.hasShopArea()) ctx.shopArea().update(dt);
    // 仅在对象仍存活时做呼吸灯，防止弱引用失效访问。
    if (auto pendingCard = m_pendingPurchase.lock()) {
         if (ctx.hasShopArea()) {
             static float time = 0.0f;
             time += dt;
             float flash = (std::sin(time * 10.0f) + 1.0f) * 0.5f; 
             sf::Uint8 gb = 100 + (sf::Uint8)(flash * 155); 
             
             pendingCard->setColor(sf::Color(255, gb, gb)); 
             
             // 仅高亮目标牌，避免玩家误判当前替换对象。
             for (auto& card : ctx.shopArea().getCards()) {
                if (card != pendingCard) card->setColor(sf::Color::White);
             }
         }
    }
}

void ShopState::draw(Game& game, sf::RenderTarget& target) {
    GameContext& ctx = game.getContext();
    if (ctx.hasShopArea()) ctx.shopArea().draw(target);
}

void ShopState::restockShop(Game& game) {
    GameContext& ctx = game.getContext();
    if (!ctx.hasShopArea() || !ctx.hasDatabase()) return;
    
    // 先清库存再补货，保证商品池来源单一。
    while (!ctx.shopArea().getCards().empty()) ctx.shopArea().removeCard(0);

    // 动态读取 ID 池，避免新增 joker 后还要改硬编码列表。
    std::vector<std::string> jokerPool = ctx.db().getAllJokerIds();
    
    // FIXME: 当前仅打印警告，后续可增加“空池兜底商品”策略。
    if (jokerPool.empty()) {
        std::cerr << "[Warning] Joker pool is empty! Check jokers.json." << std::endl;
        return;
    }

    std::vector<std::string> pickedIds = ShopRestock::PickIdsWithStdRand(jokerPool, 3);

    // 固定补 3 张，控制商店决策密度与回合节奏。
    for (const auto& id : pickedIds) {
        if (auto card = ctx.db().createJoker(id)) {
            ctx.shopArea().addCard(card);
        }
    }
    ctx.shopArea().alignCards();
}
