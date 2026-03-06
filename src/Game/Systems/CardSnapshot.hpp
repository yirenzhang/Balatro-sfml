#pragma once

#include "../Objects/CardModel.hpp"

class Card;

struct CardSnapshot {
    Suit suit = Suit::None;
    Rank rank = Rank::Two;
    int chips = 0;
    bool selected = false;
    const Card* source = nullptr;
};
