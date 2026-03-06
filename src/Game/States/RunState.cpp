#include "RunState.hpp"
#include "ShopState.hpp"
#include "../Core/Game.hpp"
#include "../Systems/HandEvaluator.hpp"
#include "../Systems/ScoringManager.hpp"
#include "../Systems/RunFlow.hpp"
#include "../Systems/CardSnapshotUtils.hpp"
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
    if (ctx.hasHandArea()) {
        while (!ctx.handArea().getCards().empty()) {
            ctx.handArea().removeCard(0);
        }
    }

    // 洗牌
    ctx.deck.initStandardDeck();
    ctx.deck.shuffle();

    // 发牌
    refillHand(game);
    markSelectionDirty();
}

void RunState::onExit([[maybe_unused]] Game& game) {
    // 离开时不需要做太多清理，数据保留在 Context 中
}

void RunState::handleEvent(Game& game, const sf::Event& event) {
    GameContext& ctx = game.getContext();

    if (event.type == sf::Event::KeyPressed) {
        // 1. 弃牌 (D)
        if (event.key.code == sf::Keyboard::D) {
            const auto& discardedSnapshots = selectedSnapshots(ctx);
            if (!discardedSnapshots.empty() && ctx.discardsLeft > 0) {

                // 计算弃牌效果
                ScoreSummary discardSummary = ScoringManager::CalculateDiscardEffect(discardedSnapshots, &ctx.jokerArea());
                // 应用效果 (加钱)
                if (discardSummary.dollars > 0) {
                    game.spawnFloatingText("+$" + std::to_string(discardSummary.dollars), 
                        sf::Vector2f(200, 600), sf::Color::Yellow);
                    std::cout << "[Effect] Earned $" << discardSummary.dollars << " from discard." << std::endl;
                }
                RunFlow::ApplyDiscard(ctx, discardSummary);
                removeSelectedCards(ctx);
                refillHand(game);
                markSelectionDirty();
                std::cout << "[Action] Discard used." << std::endl;
            }
        }

        // 2. 出牌 (Enter)
        if (event.key.code == sf::Keyboard::Enter) {
            auto selected = selectedSnapshots(ctx);
            if (!selected.empty() && ctx.handsLeft > 0) {
                playHand(game, std::move(selected));
            }
        }

    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = game.getWindow().mapPixelToCoords(sf::Mouse::getPosition(game.getWindow()));
        if (ctx.hasHandArea()) {
            auto clickedCard = ctx.handArea().getCardAt(mousePos.x, mousePos.y);
            if (clickedCard) {
                // 处理多选逻辑：如果已选则取消，未选且未满5张则选中
                if (clickedCard->isSelected()) {
                    clickedCard->toggleSelect();
                    markSelectionDirty();
                } else if (selectedSnapshots(ctx).size() < 5) {
                    clickedCard->toggleSelect();
                    markSelectionDirty();
                }
            }
        }
    }
}

void RunState::update(Game& game, float dt) {
    GameContext& ctx = game.getContext();
    
    if (ctx.hasHandArea()) ctx.handArea().update(dt);

    // 实时更新牌型信息 UI
    const auto& selected = selectedSnapshots(ctx);
    if (!selected.empty()) {
        HandResult res = HandEvaluator::Evaluate(selected);
        game.getUI().updateHandInfo(res.name, 1, res.base_chips, res.base_mult);
    } else {
        game.getUI().updateHandInfo("", 0, 0, 0);
    }
}

void RunState::draw(Game& game, sf::RenderTarget& target) {
    GameContext& ctx = game.getContext();
    if (ctx.hasHandArea()) {
        ctx.handArea().draw(target);
    }
}

void RunState::refillHand(Game& game) {
    GameContext& ctx = game.getContext();
    if (!ctx.hasHandArea() || !ctx.hasResources()) return;

    int currentCount = (int)ctx.handArea().getCards().size();
    int needed = GameContext::HAND_SIZE_LIMIT - currentCount;
    sf::Texture& deckTex = ctx.res().getTexture("deck");

    for (int i = 0; i < needed; ++i) {
        auto cardDataOpt = ctx.deck.draw();
        
        // 牌堆如果空了就停止发牌
        if (!cardDataOpt.has_value()) break;
        
        CardData data = cardDataOpt.value();
        auto card = std::make_shared<Card>(data.suit, data.rank, deckTex);
        card->setChips(data.baseChips);
        
        // 简单的发牌动画起点位置
        card->setInstantPosition(1100.0f, 700.0f);
        ctx.handArea().addCard(card);
    }
    ctx.handArea().alignCards();
    markSelectionDirty();
}

void RunState::removeSelectedCards(GameContext& ctx) {
    auto& cards = ctx.handArea().getCards();
    for (int i = cards.size() - 1; i >= 0; --i) {
        if (cards[i]->isSelected()) ctx.handArea().removeCard(i);
    }
    markSelectionDirty();
}

void RunState::playHand(Game& game, std::vector<CardSnapshot> selected) {
    GameContext& ctx = game.getContext();

    // 1. 评估牌型
    HandResult handRes = HandEvaluator::Evaluate(selected);
    
    // 2. 计算最终得分
    ScoreSummary summary = ScoringManager::CalculateFinalScore(
        handRes.base_chips,
        handRes.base_mult,
        handRes.scoring_snapshots,
        &ctx.handArea(),
        &ctx.jokerArea()
    );
    
    // 3. 生成视觉反馈 (飘字)
    for (const auto& card : selected) {
        if (!card.source) continue;
        sf::Vector2f pos = card.source->getPosition();
        pos.y -= 180.0f;
        game.spawnFloatingText("+" + std::to_string(card.chips), pos, sf::Color(100, 150, 255));
    }
    game.spawnFloatingText(handRes.name, sf::Vector2f(640, 300), sf::Color::White);

    // 4. 清理出的牌并补牌
    removeSelectedCards(ctx);
    refillHand(game);

    // 5. 更新核心数据并检测状态迁移
    const RoundTransition transition = RunFlow::ApplyPlay(ctx, summary);
    if (transition == RoundTransition::ToShop) {
        game.changeState(std::make_unique<ShopState>());
    } else if (transition == RoundTransition::GameOver) {
        // TODO: 实现 GameOverState
        std::cout << ">>> GAME OVER <<<" << std::endl;
    }
}

const std::vector<CardSnapshot>& RunState::selectedSnapshots(GameContext& ctx) {
    if (m_selectionDirty) {
        m_selectedCache = CardSnapshotUtils::BuildSelected(ctx.handArea());
        m_selectionDirty = false;
    }
    return m_selectedCache;
}
