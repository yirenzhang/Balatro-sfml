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
    
    // 每次进入运行态都重置回合资源，保证新局面可预测。
    ctx.currentScore = 0;
    ctx.handsLeft = 4;
    ctx.discardsLeft = 3;

    // 仅在未设置时回填目标分，避免覆盖外部流程写入的难度值。
    if (ctx.targetScore == 0) ctx.targetScore = 300; 

    // 先清手牌再洗牌，避免上一轮残留对象干扰抽牌顺序。
    if (ctx.hasHandArea()) {
        while (!ctx.handArea().getCards().empty()) {
            ctx.handArea().removeCard(0);
        }
    }

    // 运行态入口统一重建牌堆，确保回合起点一致。
    ctx.deck.initStandardDeck();
    ctx.deck.shuffle();

    // 进入状态后立即补满手牌，确保玩家始终可操作。
    refillHand(game);
    markSelectionDirty();
}

void RunState::onExit([[maybe_unused]] Game& game) {
    // 当前状态数据由上下文托管，此处不主动清理以支持跨状态读取。
}

void RunState::handleEvent(Game& game, const sf::Event& event) {
    GameContext& ctx = game.getContext();

    if (event.type == sf::Event::KeyPressed) {
        // 弃牌分支：先结算弃牌触发再执行移除，保持规则触发顺序一致。
        if (event.key.code == sf::Keyboard::D) {
            const auto& discardedSnapshots = selectedSnapshots(ctx);
            if (!discardedSnapshots.empty() && ctx.discardsLeft > 0) {

                ScoreSummary discardSummary = ScoringManager::CalculateDiscardEffect(discardedSnapshots, &ctx.jokerArea());
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

        // 出牌分支：仅在有选牌且有手数时进入结算。
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
                // 选择上限为 5，模拟玩法节奏并减少异常牌型输入。
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

    // 每帧预览牌型，给玩家即时反馈，减少试错成本。
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
        
        // 牌堆耗尽时提前终止，防止无效构造导致空牌异常。
        if (!cardDataOpt.has_value()) break;
        
        CardData data = cardDataOpt.value();
        auto card = std::make_shared<Card>(data.suit, data.rank, deckTex);
        card->setChips(data.baseChips);
        
        // 从固定发牌起点入场，保持动画空间一致性。
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

    // 先评估牌型，再把结果作为计分输入，保证规则入口单一。
    HandResult handRes = HandEvaluator::Evaluate(selected);
    
    // 计分阶段需要手牌区和 Joker 区共同参与触发链。
    ScoreSummary summary = ScoringManager::CalculateFinalScore(
        handRes.base_chips,
        handRes.base_mult,
        handRes.scoring_snapshots,
        &ctx.handArea(),
        &ctx.jokerArea()
    );
    
    // 结算后再生成反馈，避免视觉与逻辑结果不一致。
    for (const auto& card : selected) {
        if (!card.source) continue;
        sf::Vector2f pos = card.source->getPosition();
        pos.y -= 180.0f;
        game.spawnFloatingText("+" + std::to_string(card.chips), pos, sf::Color(100, 150, 255));
    }
    game.spawnFloatingText(handRes.name, sf::Vector2f(640, 300), sf::Color::White);

    // 先移除再补牌，确保手牌数量统计正确。
    removeSelectedCards(ctx);
    refillHand(game);

    // 统一用 RunFlow 决定迁移，避免状态判断散落在多个分支。
    const RoundTransition transition = RunFlow::ApplyPlay(ctx, summary);
    if (transition == RoundTransition::ToShop) {
        game.changeState(std::make_unique<ShopState>());
    } else if (transition == RoundTransition::GameOver) {
        // TODO: 补充独立 GameOverState，并迁移当前日志提示。
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
