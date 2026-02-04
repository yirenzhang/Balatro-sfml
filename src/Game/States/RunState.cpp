#include "RunState.hpp"
#include "ShopState.hpp"
#include "../Core/Game.hpp"
#include "../Systems/HandEvaluator.hpp"
#include "../Systems/ScoringManager.hpp"
#include "../Systems/ResourceManager.hpp"
#include <iostream>

void RunState::onEnter(Game& game) {
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

void RunState::onExit([[maybe_unused]] Game& game) {
    // 离开时不需要做太多清理，数据保留在 Context 中
}

void RunState::handleEvent(Game& game, const sf::Event& event) {
    GameContext& ctx = game.getContext();

    if (event.type == sf::Event::KeyPressed) {
        // 1. 弃牌 (D)
        if (event.key.code == sf::Keyboard::D) {
            auto selected = ctx.area_hand->getSelectedCards();
            if (!selected.empty() && ctx.discardsLeft > 0) {
                // 计算弃牌效果
                ScoreSummary discardSummary = ScoringManager::CalculateDiscardEffect(selected, ctx.area_jokers);
                // 应用效果 (加钱)
                if (discardSummary.dollars > 0) {
                    ctx.money += discardSummary.dollars;
                    game.spawnFloatingText("+$" + std::to_string(discardSummary.dollars), 
                        sf::Vector2f(200, 600), sf::Color::Yellow);
                    std::cout << "[Effect] Earned $" << discardSummary.dollars << " from discard." << std::endl;
                }
                // 执行弃牌动作
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
                playHand(game, selected);
            }
        }

        // [CHEAT] 按 M 加钱
        if (event.key.code == sf::Keyboard::M) {
            ctx.money += 100;
            // 播放飘字特效，给予视觉反馈
            game.spawnFloatingText("CHEAT: +$100", sf::Vector2f(1050, 600), sf::Color::Yellow);
            std::cout << "[CHEAT] Money added. Current: " << ctx.money << std::endl;
        }

        // [CHEAT] 按 P 加分
        if (event.key.code == sf::Keyboard::P) {
            ctx.currentScore += 10000;
            game.spawnFloatingText("CHEAT: +10,000 Score", sf::Vector2f(200, 240), sf::Color::Magenta);
            std::cout << "[CHEAT] Score added. Current: " << ctx.currentScore << std::endl;

            // 可选：如果加上分数后超过目标，直接触发过关逻辑吗？
            // 建议：这里只加分，让玩家手动打出一张牌或者按 Next 来结算，这样更安全。
            // 如果你想直接过关，可以复制 playHand 里的状态切换代码到这里。
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
        if (ctx.area_hand) {
            auto clickedCard = ctx.area_hand->getCardAt(mousePos.x, mousePos.y);
            if (clickedCard) {
                // 处理多选逻辑：如果已选则取消，未选且未满5张则选中
                if (clickedCard->isSelected()) {
                    clickedCard->toggleSelect();
                } else if (ctx.area_hand->getSelectedCards().size() < 5) {
                    clickedCard->toggleSelect();
                }
            }
        }
    }
}

void RunState::update(Game& game, float dt) {
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

void RunState::draw(Game& game, sf::RenderTarget& target) {
    GameContext& ctx = game.getContext();
    if (ctx.area_hand) {
        ctx.area_hand->draw(target);
    }
}

void RunState::refillHand(Game& game) {
    GameContext& ctx = game.getContext();
    if (!ctx.area_hand) return;

    int currentCount = (int)ctx.area_hand->getCards().size();
    int needed = GameContext::HAND_SIZE_LIMIT - currentCount;
    sf::Texture& deckTex = ResourceManager::Instance().getTexture("deck");

    for (int i = 0; i < needed; ++i) {
        auto cardDataOpt = ctx.deck.draw();
        
        // 牌堆如果空了就停止发牌
        if (!cardDataOpt.has_value()) break;
        
        CardData data = cardDataOpt.value();
        auto card = std::make_shared<Card>(data.suit, data.rank, deckTex);
        card->setChips(data.baseChips);
        
        // 简单的发牌动画起点位置
        card->setInstantPosition(1100.0f, 700.0f);
        ctx.area_hand->addCard(card);
    }
    ctx.area_hand->alignCards();
}

void RunState::removeSelectedCards(GameContext& ctx) {
    auto& cards = ctx.area_hand->getCards();
    for (int i = cards.size() - 1; i >= 0; --i) {
        if (cards[i]->isSelected()) ctx.area_hand->removeCard(i);
    }
}

void RunState::playHand(Game& game, std::vector<Card*> selected) {
    GameContext& ctx = game.getContext();
    
    // 1. 评估牌型
    HandResult handRes = HandEvaluator::Evaluate(selected);
    
    // 2. 计算最终得分
    ScoreSummary summary = ScoringManager::CalculateFinalScore(handRes, ctx.area_hand, ctx.area_jokers);
    
    // 3. 生成视觉反馈 (飘字)
    for (Card* card : selected) {
        sf::Vector2f pos = card->getPosition();
        pos.y -= 180.0f;
        game.spawnFloatingText("+" + std::to_string(card->getChips()), pos, sf::Color(100, 150, 255));
    }
    game.spawnFloatingText(handRes.name, sf::Vector2f(640, 300), sf::Color::White);

    // 4. 更新核心数据
    ctx.currentScore += summary.final_score;
    ctx.handsLeft--;

    // 5. 清理出的牌并补牌
    removeSelectedCards(ctx);
    refillHand(game);

    // 6. 状态检测：过关或失败
    if (ctx.currentScore >= ctx.targetScore) {
        ctx.money += 5; // 基础过关奖励
        ctx.targetScore = (long long)(ctx.targetScore * 1.5f); // 难度提升
        
        // 核心修改点：这里可以安全地包含 ShopState 了
        game.changeState(std::make_unique<ShopState>());
    }
    else if (ctx.handsLeft <= 0) {
        // TODO: 实现 GameOverState
        // game.changeState(std::make_unique<GameOverState>());
        std::cout << ">>> GAME OVER <<<" << std::endl;
    }
}