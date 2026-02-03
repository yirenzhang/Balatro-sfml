#pragma once
#include "RunState.hpp"
#include "ShopState.hpp"
#include "../Systems/GameDatabase.hpp"
#include "../Core/Game.hpp"
#include <iostream>
#include <cmath>

// RunState::playHand 保持不变，略过...
inline void RunState::playHand(Game& game, std::vector<Card*> selected) {
    // ... (保持原样) ...
    GameContext& ctx = game.getContext();
    HandResult handRes = HandEvaluator::Evaluate(selected);
    ScoreSummary summary = ScoringManager::CalculateFinalScore(handRes, ctx.area_jokers);
    
    for (Card* card : selected) {
        sf::Vector2f pos = card->getPosition();
        pos.y -= 180.0f;
        game.spawnFloatingText("+" + std::to_string(card->getChips()), pos, sf::Color(100, 150, 255));
    }
    game.spawnFloatingText(handRes.name, sf::Vector2f(640, 300), sf::Color::White);

    ctx.currentScore += summary.final_score;
    ctx.handsLeft--;

    removeSelectedCards(ctx);
    refillHand(game);

    if (ctx.currentScore >= ctx.targetScore) {
        ctx.money += 5;
        ctx.targetScore = (long long)(ctx.targetScore * 1.5f); 
        game.changeState(std::make_unique<ShopState>());
    }
    else if (ctx.handsLeft <= 0) {
        // game.changeState(std::make_unique<GameOverState>());
    }
}


// ShopState::handleEvent 核心修改点
inline void ShopState::handleEvent(Game& game, const sf::Event& event) {
    GameContext& ctx = game.getContext();
    UIManager& ui = game.getUI();

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::N) {
            game.changeState(std::make_unique<RunState>());
            return;
        }
        if (event.key.code == sf::Keyboard::R) {
            int rerollCost = 5;
            if (ctx.money >= rerollCost) {
                ctx.money -= rerollCost;
                restockShop(game);
                m_pendingPurchase = nullptr;
            }
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
        
        // 1. 点击 Joker 区 (替换逻辑)
        auto clickedJoker = ctx.area_jokers->getCardAt(mousePos.x, mousePos.y);
        
        if (m_pendingPurchase && clickedJoker) {
            int cost = m_pendingPurchase->getCost();
            if (ctx.money >= cost) {
                ctx.money -= cost;
                
                // 1. 移除旧卡 (不需要保留，直接 remove 即可销毁)
                // 这里调用的是 removeCard(Card*)，如果你删了那个重载，可以用 removeCard(index)
                // 或者用 takeCard 然后不接收返回值，让它析构。
                // 推荐：takeCard(clickedJoker.get()); // 让它自然销毁
                ctx.area_jokers->takeCard(clickedJoker.get());
                
                // 2. [修复] 使用 takeCard 获取新卡的所有权
                if (auto newCard = ctx.area_shop->takeCard(m_pendingPurchase)) {
                    newCard->setColor(sf::Color::White);
                    // 3. 安全地添加到 Joker 区
                    ctx.area_jokers->addCard(newCard);
                }

                m_pendingPurchase = nullptr;
                ui.setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
                
                ctx.area_jokers->alignCards();
                ctx.area_shop->alignCards();
            }
            return;
        }

        // 2. 点击 商店 区 (购买逻辑)
        auto clickedShopCard = ctx.area_shop->getCardAt(mousePos.x, mousePos.y);
        if (clickedShopCard) {
            int cost = clickedShopCard->getCost();
            if (ctx.money < cost) return;

            int jokerCount = (int)ctx.area_jokers->getCards().size();
            
            // 直接购买
            if (jokerCount < 5) {
                ctx.money -= cost;
                
                // [修复] 使用 takeCard 获取所有权
                if (auto newCard = ctx.area_shop->takeCard(clickedShopCard.get())) {
                    newCard->setColor(sf::Color::White);
                    ctx.area_jokers->addCard(newCard);
                }
                
                if (m_pendingPurchase) {
                    m_pendingPurchase->setColor(sf::Color::White);
                    m_pendingPurchase = nullptr;
                    ui.setShopMessage("SHOP PHASE\n[Left Click] Buy Joker\n[N] Next Round", sf::Color::Yellow);
                }
                ctx.area_jokers->alignCards();
                ctx.area_shop->alignCards();
            }
            // 槽位满 -> 选中待替换
            else {
                if (m_pendingPurchase) m_pendingPurchase->setColor(sf::Color::White);
                m_pendingPurchase = clickedShopCard.get(); // 记录裸指针用于高亮
                ui.setShopMessage("SELECT A JOKER TO REPLACE\n[Right Click] Cancel", sf::Color::Red);
            }
        }
    }
}