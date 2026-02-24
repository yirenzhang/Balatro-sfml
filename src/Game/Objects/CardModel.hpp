#pragma once

#include <memory>
#include <string>

class IEffect;

enum class Suit { Spades, Hearts, Clubs, Diamonds, None };
enum class Rank {
    Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
    Jack = 11, Queen = 12, King = 13, Ace = 14
};

enum class CardType {
    PlayingCard,
    Joker
};

struct CardModel {
    CardType type = CardType::PlayingCard;
    Suit suit = Suit::None;
    Rank rank = Rank::Two;

    bool isHovered = false;
    bool isSelected = false;

    std::shared_ptr<IEffect> effect = nullptr;
    std::string abilityName = "Card";
    std::string description;

    int chips = 0;
    int cost = 0;
};
