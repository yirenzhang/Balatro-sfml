#include "ShopState.hpp"
#include "RunState.hpp"
#include "../Core/Game.hpp"
#include "../Systems/GameDatabase.hpp"
#include <cmath>
#include <iostream>

void ShopState::onEnter(Game& game) {
    game.getContext().state = GameState::Shop;
    std::cout << ">>> Enter Shop State <<<" << std::endl;
    
    // 进入商店时自动进货
    restockShop(game);
    game.getUI().setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
}

void ShopState::onExit([[maybe_unused]] Game& game) {
    // 离开状态时，重置弱引用
    m_pendingPurchase.reset();
}

void ShopState::handleEvent(Game& game, const sf::Event& event) {
    GameContext& ctx = game.getContext();
    UIManager& ui = game.getUI();

    if (event.type == sf::Event::KeyPressed) {
        // [N] 下一轮 -> 切换回 RunState
        if (event.key.code == sf::Keyboard::N) {
            game.changeState(std::make_unique<RunState>());
            return;
        }
        
        // [R] 刷新商店 (Reroll)
        if (event.key.code == sf::Keyboard::R) {
            int rerollCost = 5;
            if (ctx.money >= rerollCost) {
                ctx.money -= rerollCost;
                restockShop(game);
                m_pendingPurchase.reset();
            }
        }

        // [CHEAT] 按 M 加钱
        if (event.key.code == sf::Keyboard::M) {
            ctx.money += 100;
            game.spawnFloatingText("CHEAT: +$100", sf::Vector2f(1050, 600), sf::Color::Yellow);
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
        
        // 情况 A: 点击 Joker 区 (可能涉及替换)
        auto clickedJoker = ctx.area_jokers->getCardAt(mousePos.x, mousePos.y);
        
        // [关键] 尝试锁定 weak_ptr 获取可用的 shared_ptr
        // 如果对象存在，pendingCard 将非空；如果已被销毁，则为空。
        auto pendingCard = m_pendingPurchase.lock();

        if (pendingCard && clickedJoker) {
            int cost = pendingCard->getCost();
            if (ctx.money >= cost) {
                ctx.money -= cost;
                
                // 1. 移除 Joker 区旧卡
                ctx.area_jokers->takeCard(clickedJoker.get());
                
                // 2. 从商店区获取新卡所有权并放入 Joker 区
                if (auto newCard = ctx.area_shop->takeCard(pendingCard.get())) {
                    newCard->setColor(sf::Color::White);
                    ctx.area_jokers->addCard(newCard);
                }

                // 3. 重置状态
                m_pendingPurchase.reset();
                ui.setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
                
                ctx.area_jokers->alignCards();
                ctx.area_shop->alignCards();
            }
            return;
        }

        // 情况 B: 点击 商店 区 (购买逻辑)
        auto clickedShopCard = ctx.area_shop->getCardAt(mousePos.x, mousePos.y);
        if (clickedShopCard) {
            int cost = clickedShopCard->getCost();
            
            // 钱不够直接忽略
            if (ctx.money < cost) return;

            int jokerCount = (int)ctx.area_jokers->getCards().size();
            
            // 槽位未满 -> 直接购买
            if (jokerCount < 5) {
                ctx.money -= cost;
                
                if (auto newCard = ctx.area_shop->takeCard(clickedShopCard.get())) {
                    newCard->setColor(sf::Color::White);
                    ctx.area_jokers->addCard(newCard);
                }
                
                // 如果之前有选中的卡，取消其高亮（如果它还活着）
                if (auto oldPending = m_pendingPurchase.lock()) {
                    oldPending->setColor(sf::Color::White);
                }
                m_pendingPurchase.reset();

                ui.setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
                ctx.area_jokers->alignCards();
                ctx.area_shop->alignCards();
            }
            // 槽位已满 -> 选中该卡，提示玩家选择替换对象
            else {
                // 取消旧卡高亮
                if (auto oldPending = m_pendingPurchase.lock()) {
                    oldPending->setColor(sf::Color::White);
                }
                
                // [关键] 记录新的 weak_ptr
                // 隐式转换：shared_ptr (clickedShopCard) -> weak_ptr
                m_pendingPurchase = clickedShopCard; 
                
                ui.setShopMessage("SELECT A JOKER TO REPLACE\n[Right Click] Cancel", sf::Color::Red);
            }
        }
    }

    // 右键取消选中
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
    
    if (ctx.area_shop) ctx.area_shop->update(dt);
    // [关键] 安全的呼吸灯效果
    // 先锁定，确认对象存在后再操作
    if (auto pendingCard = m_pendingPurchase.lock()) {
         if (ctx.area_shop) {
             static float time = 0.0f;
             time += dt;
             float flash = (std::sin(time * 10.0f) + 1.0f) * 0.5f; 
             sf::Uint8 gb = 100 + (sf::Uint8)(flash * 155); 
             
             pendingCard->setColor(sf::Color(255, gb, gb)); 
             
             // 确保其他卡片颜色正常
             for (auto& card : ctx.area_shop->getCards()) {
                if (card != pendingCard) card->setColor(sf::Color::White);
             }
         }
    }
}

void ShopState::draw(Game& game, sf::RenderTarget& target) {
    GameContext& ctx = game.getContext();
    if (ctx.area_shop) ctx.area_shop->draw(target);
    
    // 绘制商店特定 UI
    game.getUI().draw(target, GameState::Shop);
}

void ShopState::restockShop(Game& game) {
    GameContext& ctx = game.getContext();
    if (!ctx.area_shop) return;
    
    // 清空当前商品
    while (!ctx.area_shop->getCards().empty()) ctx.area_shop->removeCard(0);

    // [优化] 动态获取所有 Joker ID
    // 这样只要 jokers.json 里有的牌，都有机会刷出来
    std::vector<std::string> jokerPool = GameDatabase::Instance().getAllJokerIds();
    
    // 安全检查：如果 JSON 加载失败导致池子为空，直接返回防止除零错误
    if (jokerPool.empty()) {
        std::cerr << "[Warning] Joker pool is empty! Check jokers.json." << std::endl;
        return;
    }

    // 随机生成 3 张 Joker
    for (int i = 0; i < 3; ++i) {
        std::string id = jokerPool[std::rand() % jokerPool.size()];
        if (auto card = GameDatabase::Instance().createJoker(id)) {
            ctx.area_shop->addCard(card);
        }
    }
    ctx.area_shop->alignCards();
}