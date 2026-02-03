#pragma once
#include "IGameState.hpp"
#include "../Core/Game.hpp"
#include "../Systems/HandEvaluator.hpp"
#include "../Systems/ScoringManager.hpp"
#include "../Systems/ResourceManager.hpp"
#include <iostream>

class RunState : public IGameState {
public:
    void onEnter(Game& game) override {
        game.getContext().state = GameState::Run;
        std::cout << ">>> Enter Run State <<<" << std::endl;
        GameContext& ctx = game.getContext();
        
        // 重置回合数据
        ctx.currentScore = 0;
        ctx.handsLeft = 4;
        ctx.discardsLeft = 3;
        // 如果是第一轮，初始化目标分数
        if (ctx.targetScore == 0) ctx.targetScore = 300; 

        // 清空手牌区
        if (ctx.area_hand) {
            while (!ctx.area_hand->getCards().empty()) {
                ctx.area_hand->removeCard(0);
            }
        }

        // 洗牌
        ctx.deck.initStandardDeck();
        ctx.deck.shuffle();

        // 发牌
        refillHand(game);
    }

    void onExit(Game& game) override {
        // 离开时不需要做太多清理，数据保留在 Context 中
    }

    void handleEvent(Game& game, const sf::Event& event) override {
        GameContext& ctx = game.getContext();

        if (event.type == sf::Event::KeyPressed) {
            // 1. 弃牌 (D)
            if (event.key.code == sf::Keyboard::D) {
                auto selected = ctx.area_hand->getSelectedCards();
                if (!selected.empty() && ctx.discardsLeft > 0) {
                    ctx.discardsLeft--;
                    removeSelectedCards(ctx);
                    refillHand(game);
                    std::cout << "[Action] Discard used." << std::endl;
                }
            }

            // 2. 出牌 (Enter)
            if (event.key.code == sf::Keyboard::Enter) {
                auto selected = ctx.area_hand->getSelectedCards();
                if (!selected.empty() && ctx.handsLeft > 0) {
                    // 调用核心出牌逻辑 (实现在 StateDefinitions.hpp)
                    playHand(game, selected);
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
            if (ctx.area_hand) {
                auto clickedCard = ctx.area_hand->getCardAt(mousePos.x, mousePos.y);
                if (clickedCard) {
                    if (clickedCard->isSelected()) {
                        clickedCard->toggleSelect();
                    } else if (ctx.area_hand->getSelectedCards().size() < 5) {
                        clickedCard->toggleSelect();
                    }
                }
            }
        }
    }

    void update(Game& game, float dt) override {
        GameContext& ctx = game.getContext();
        
        if (ctx.area_hand) ctx.area_hand->update(dt);

        // 实时更新牌型信息 UI
        auto selected = ctx.area_hand->getSelectedCards();
        if (!selected.empty()) {
            HandResult res = HandEvaluator::Evaluate(selected);
            game.getUI().updateHandInfo(res.name, 1, res.base_chips, res.base_mult);
        } else {
            game.getUI().updateHandInfo("", 0, 0, 0);
        }
    }

    void draw(Game& game, sf::RenderTarget& target) override {
    GameContext& ctx = game.getContext();
        if (ctx.area_hand) {
            // std::cout << "RunState: Drawing hand. Cards count: " << ctx.area_hand->getCards().size() << std::endl; // [Debug 2]
            ctx.area_hand->draw(target);
        } else {
            std::cout << "RunState: area_hand is NULL!" << std::endl; // [Debug Warning]
        }
    }

    // 辅助函数：补牌
    void refillHand(Game& game) {
        GameContext& ctx = game.getContext();
        if (!ctx.area_hand) return;

        int currentCount = (int)ctx.area_hand->getCards().size();
        int needed = GameContext::HAND_SIZE_LIMIT - currentCount;
        sf::Texture& deckTex = ResourceManager::Instance().getTexture("deck");

        for (int i = 0; i < needed; ++i) {
            auto cardDataOpt = ctx.deck.draw();
            if (!cardDataOpt.has_value()) break;
            CardData data = cardDataOpt.value();
            auto card = std::make_shared<Card>(data.suit, data.rank, deckTex);
            card->setChips(data.baseChips);
            card->setInstantPosition(1100.0f, 700.0f);
            ctx.area_hand->addCard(card);
        }
        ctx.area_hand->alignCards();
    }

private:
    void removeSelectedCards(GameContext& ctx) {
        auto& cards = ctx.area_hand->getCards();
        for (int i = cards.size() - 1; i >= 0; --i) {
            if (cards[i]->isSelected()) ctx.area_hand->removeCard(i);
        }
    }

    // [关键] 此函数会导致状态切换，必须在 StateDefinitions.hpp 中实现
    void playHand(Game& game, std::vector<Card*> selected);
};