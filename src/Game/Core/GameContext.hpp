#pragma once
#include "../Effects/EffectContext.hpp"
#include "Deck.hpp"

class CardArea;
class Card;

enum class GameState {
    Menu,
    Run,
    Shop,
    GameOver,
    Victory
};

struct GameContext {
    GameState state = GameState::Menu;
    
    Deck deck;

    CardArea* area_hand = nullptr;
    CardArea* area_jokers = nullptr;
    CardArea* area_shop = nullptr; 

    TriggerType trigger = TriggerType::Individual;
    Card* other_card = nullptr; 
    long long current_chips = 0;

    int handsLeft = 4;
    int discardsLeft = 3;
    long long currentScore = 0;
    long long targetScore = 300;
    
    int money = 4; // 初始 4 块钱
    
    static const int HAND_SIZE_LIMIT = 8;
};