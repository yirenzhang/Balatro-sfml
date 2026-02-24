#include "JokerEffectFactory.hpp"

#include "../Effects/JokerEffects.hpp"
#include "../Objects/Card.hpp"

namespace {

Suit parseSuit(const std::string& suitStr) {
    if (suitStr == "Hearts") return Suit::Hearts;
    if (suitStr == "Clubs") return Suit::Clubs;
    if (suitStr == "Diamonds") return Suit::Diamonds;
    return Suit::Spades;
}

Rank parseRank(const std::string& rankStr) {
    if (rankStr == "2") return Rank::Two;
    if (rankStr == "3") return Rank::Three;
    if (rankStr == "4") return Rank::Four;
    if (rankStr == "5") return Rank::Five;
    if (rankStr == "6") return Rank::Six;
    if (rankStr == "7") return Rank::Seven;
    if (rankStr == "8") return Rank::Eight;
    if (rankStr == "9") return Rank::Nine;
    if (rankStr == "10") return Rank::Ten;
    if (rankStr == "Jack") return Rank::Jack;
    if (rankStr == "Queen") return Rank::Queen;
    if (rankStr == "King") return Rank::King;
    return Rank::Ace;
}

} // namespace

namespace JokerEffectFactory {

std::shared_ptr<IEffect> Create(const std::string& effectId, const nlohmann::json& params) {
    if (effectId == "SimpleMult") {
        int amount = params.value("amount", 0);
        return std::make_shared<SimpleMultEffect>(amount);
    }

    if (effectId == "SuitMult") {
        int amount = params.value("amount", 0);
        std::string suitStr = params.value("suit", "Spades");
        return std::make_shared<SuitMultEffect>(amount, parseSuit(suitStr), suitStr);
    }

    if (effectId == "AbstractJoker") {
        int amount = params.value("amount", 0);
        return std::make_shared<AbstractJokerEffect>(amount);
    }

    if (effectId == "DiscardRebate") {
        int amount = params.value("amount", 0);
        std::string rankStr = params.value("rank", "Ace");
        return std::make_shared<DiscardRebateEffect>(amount, parseRank(rankStr));
    }

    return nullptr;
}

} // namespace JokerEffectFactory
