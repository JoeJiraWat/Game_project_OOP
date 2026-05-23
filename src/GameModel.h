#pragma once

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>

class GameManager;

enum class CardType {
    Boom,
    Defuse,
    Skip,
    Favor,
    DrawFromBottom,
    SeeTheFuture,
    Shuffle,
    Nope,
    CatCard
};

enum class Effect {
    KaBoom,
    DefuseABoom,
    SkipTurn,
    Favor,
    SeeCard,
    ShuffleDeck,
    CancelEffect,
    DrawFromTheBottom,
    NormalCard
};

enum class CatType {
    Cat1,
    Cat2,
    Cat3,
    Cat4,
    Cat5
};

struct CardData {
    std::string Name;
    int Quantity;
    CardType Card_type;
    std::string Description;
    Effect Effect_type;
};

inline std::string toString(CardType type)
{
    switch (type) {
    case CardType::Boom:
        return "ระเบิด";
    case CardType::Defuse:
        return "กู้ระเบิด";
    case CardType::Skip:
        return "ข้ามตา";
    case CardType::Favor:
        return "ขอความช่วยเหลือ";
    case CardType::DrawFromBottom:
        return "จั่วจากล่างสุด";
    case CardType::SeeTheFuture:
        return "ดูอนาคต";
    case CardType::Shuffle:
        return "สับกอง";
    case CardType::Nope:
        return "ยกเลิก";
    case CardType::CatCard:
        return "การ์ดแมว";
    }
    return "ไม่ทราบ";
}

inline std::string toString(CatType type)
{
    switch (type) {
    case CatType::Cat1:
        return "แมว 1";
    case CatType::Cat2:
        return "แมว 2";
    case CatType::Cat3:
        return "แมว 3";
    case CatType::Cat4:
        return "แมว 4";
    case CatType::Cat5:
        return "แมว 5";
    }
    return "แมว";
}

inline std::array<CatType, 5> allCatTypes()
{
    return { CatType::Cat1, CatType::Cat2, CatType::Cat3, CatType::Cat4, CatType::Cat5 };
}

class Card {
public:
    virtual ~Card() = default;

    CardType type() const { return field_type_; }
    Effect effect() const { return effect_; }
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    virtual std::optional<CatType> catType() const { return std::nullopt; }

    virtual void OnUse(GameManager& gm) = 0;
    virtual std::unique_ptr<Card> clone() const = 0;

protected:
    Card(CardType type, std::string name, std::string description, Effect effect)
        : field_type_(type)
        , name_(std::move(name))
        , description_(std::move(description))
        , effect_(effect)
    {
    }

    CardType field_type_;
    std::string name_;
    std::string description_;
    Effect effect_;
};

class Boom : public Card {
public:
    Boom()
        : Card(CardType::Boom, "ระเบิด", "ถ้าจั่วได้แล้วไม่มีการ์ดกู้ระเบิด คุณจะออกจากเกม", Effect::KaBoom)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<Boom>(*this); }
};

class Defuse : public Card {
public:
    Defuse()
        : Card(CardType::Defuse, "กู้ระเบิด", "ใช้โดยอัตโนมัติเมื่อคุณจั่วเจอระเบิด", Effect::DefuseABoom)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<Defuse>(*this); }
};

class Skip : public Card {
public:
    Skip()
        : Card(CardType::Skip, "ข้ามตา", "จบตาของคุณทันทีโดยไม่ต้องจั่วการ์ด", Effect::SkipTurn)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<Skip>(*this); }
};

class Favor : public Card {
public:
    Favor()
        : Card(CardType::Favor, "ขอความช่วยเหลือ", "เลือกผู้เล่นหนึ่งคน แล้วขโมยการ์ดแบบสุ่มจากเขา", Effect::Favor)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<Favor>(*this); }
};

class SeeTheFuture : public Card {
public:
    SeeTheFuture()
        : Card(CardType::SeeTheFuture, "ดูอนาคต", "แอบดูการ์ด 3 ใบบนสุดของกองจั่ว", Effect::SeeCard)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<SeeTheFuture>(*this); }
};

class Shuffle : public Card {
public:
    Shuffle()
        : Card(CardType::Shuffle, "สับกอง", "สับกองจั่วใหม่ทั้งหมด", Effect::ShuffleDeck)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<Shuffle>(*this); }
};

class Nope : public Card {
public:
    Nope()
        : Card(CardType::Nope, "ยกเลิก", "ใช้ยกเลิกการ์ดขอความช่วยเหลือหรือคอมโบแมวเมื่อระบบถาม", Effect::CancelEffect)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<Nope>(*this); }
};

class DrawFromBottom : public Card {
public:
    DrawFromBottom()
        : Card(CardType::DrawFromBottom, "จั่วจากล่างสุด", "จั่วการ์ดใบล่างสุดของกอง แล้วจบตาของคุณ", Effect::DrawFromTheBottom)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<DrawFromBottom>(*this); }
};

class CatCard : public Card {
public:
    explicit CatCard(CatType type)
        : Card(CardType::CatCard, toString(type), "ใช้รวมเป็นคอมโบแมวเพื่อขโมยการ์ดจากผู้เล่นอื่น", Effect::NormalCard)
        , quantity_Cat_card(1)
        , Cat_type(type)
    {
    }

    void OnUse(GameManager& gm) override;
    std::unique_ptr<Card> clone() const override { return std::make_unique<CatCard>(*this); }
    std::optional<CatType> catType() const override { return Cat_type; }

    int quantity_Cat_card;
    CatType Cat_type;
};

inline std::unique_ptr<Card> createCard(CardType type, CatType catType = CatType::Cat1)
{
    switch (type) {
    case CardType::Boom:
        return std::make_unique<Boom>();
    case CardType::Defuse:
        return std::make_unique<Defuse>();
    case CardType::Skip:
        return std::make_unique<Skip>();
    case CardType::Favor:
        return std::make_unique<Favor>();
    case CardType::DrawFromBottom:
        return std::make_unique<DrawFromBottom>();
    case CardType::SeeTheFuture:
        return std::make_unique<SeeTheFuture>();
    case CardType::Shuffle:
        return std::make_unique<Shuffle>();
    case CardType::Nope:
        return std::make_unique<Nope>();
    case CardType::CatCard:
        return std::make_unique<CatCard>(catType);
    }
    return std::make_unique<CatCard>(catType);
}

class Player {
public:
    Player(std::string playerName = "ผู้เล่น")
        : name(std::move(playerName))
    {
    }

    Player(Player&&) noexcept = default;
    Player& operator=(Player&&) noexcept = default;
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    std::string rename(const std::string& newName)
    {
        name = newName;
        return name;
    }

    void draw(std::unique_ptr<Card> card)
    {
        if (card) {
            myCard.push_back(std::move(card));
        }
    }

    void useCard(int cardIndex)
    {
        if (cardIndex >= 0 && cardIndex < static_cast<int>(myCard.size())) {
            myCard.erase(myCard.begin() + cardIndex);
        }
    }

    std::unique_ptr<Card> removeCardAt(int cardIndex)
    {
        if (cardIndex < 0 || cardIndex >= static_cast<int>(myCard.size())) {
            return nullptr;
        }
        auto card = std::move(myCard[cardIndex]);
        myCard.erase(myCard.begin() + cardIndex);
        return card;
    }

    bool hasCard(CardType type) const
    {
        return std::any_of(myCard.begin(), myCard.end(), [type](const auto& card) {
            return card->type() == type;
        });
    }

    int findFirst(CardType type) const
    {
        for (int i = 0; i < static_cast<int>(myCard.size()); ++i) {
            if (myCard[i]->type() == type) {
                return i;
            }
        }
        return -1;
    }

    int count(CardType type) const
    {
        return static_cast<int>(std::count_if(myCard.begin(), myCard.end(), [type](const auto& card) {
            return card->type() == type;
        }));
    }

    int countCat(CatType type) const
    {
        int total = 0;
        for (const auto& card : myCard) {
            if (card->catType().has_value() && card->catType().value() == type) {
                ++total;
            }
        }
        return total;
    }

    int differentCatCount() const
    {
        int total = 0;
        for (CatType type : allCatTypes()) {
            if (countCat(type) > 0) {
                ++total;
            }
        }
        return total;
    }

    std::optional<CatType> firstCatWithCount(int requiredCount) const
    {
        for (CatType type : allCatTypes()) {
            if (countCat(type) >= requiredCount) {
                return type;
            }
        }
        return std::nullopt;
    }

    std::vector<int> catIndices(CatType type, int requiredCount) const
    {
        std::vector<int> indices;
        for (int i = 0; i < static_cast<int>(myCard.size()) && static_cast<int>(indices.size()) < requiredCount; ++i) {
            if (myCard[i]->catType().has_value() && myCard[i]->catType().value() == type) {
                indices.push_back(i);
            }
        }
        return indices;
    }

    std::vector<int> oneOfEachCatIndices() const
    {
        std::vector<int> indices;
        for (CatType catType : allCatTypes()) {
            for (int i = 0; i < static_cast<int>(myCard.size()); ++i) {
                if (myCard[i]->catType().has_value() && myCard[i]->catType().value() == catType) {
                    indices.push_back(i);
                    break;
                }
            }
        }
        return indices;
    }

    std::unique_ptr<Card> takeRandomCard(std::mt19937& rng)
    {
        if (myCard.empty()) {
            return nullptr;
        }
        std::uniform_int_distribution<int> dist(0, static_cast<int>(myCard.size()) - 1);
        return removeCardAt(dist(rng));
    }

    std::unique_ptr<Card> takeNamedCard(const std::string& targetCardName)
    {
        for (int i = 0; i < static_cast<int>(myCard.size()); ++i) {
            if (myCard[i]->name() == targetCardName) {
                return removeCardAt(i);
            }
        }
        return nullptr;
    }

    std::string name;
    bool isActive = true;
    std::vector<std::unique_ptr<Card>> myCard;
    bool isTurn = false;
    bool isAlive = true;
};

class NetworkManager {
public:
    static NetworkManager& instance()
    {
        static NetworkManager manager;
        return manager;
    }

    void create_room()
    {
        roomCode_ = "LOCAL-ROOM";
        eventLog_.push_back("สร้างห้องในเครื่อง LOCAL-ROOM แล้ว");
    }

    void join_room(const std::string& code)
    {
        roomCode_ = code;
        eventLog_.push_back("เข้าห้อง " + code + " แล้ว");
    }

    void rpc_send_card_action(int card_index)
    {
        eventLog_.push_back("ส่งคำสั่งเล่นการ์ดในเครื่อง หมายเลข " + std::to_string(card_index));
    }

    const std::vector<std::string>& events() const { return eventLog_; }

private:
    NetworkManager() = default;

    std::string ENetMultiplayerPeer_peer_;
    std::vector<Player*> Player_;
    std::string roomCode_;
    std::vector<std::string> eventLog_;
};

class SpecialRule {
public:
    SpecialRule(std::string name, int requiredCount)
        : ruleName(std::move(name))
        , requiredCardCount(requiredCount)
    {
    }

    virtual ~SpecialRule() = default;

    virtual bool checkCondition(const Player& player) const = 0;
    virtual void executeAction(GameManager& gm, int playerId, int targetPlayerId, const std::string& targetCardName) = 0;

    bool checkConditionFlag = false;
    std::string ruleName;
    int requiredCardCount;
};

class TwoIdenticalCatsRule : public SpecialRule {
public:
    TwoIdenticalCatsRule()
        : SpecialRule("แมวเหมือนกัน 2 ใบ", 2)
    {
    }

    bool checkCondition(const Player& player) const override
    {
        return player.firstCatWithCount(requiredCardCount).has_value();
    }

    void executeAction(GameManager& gm, int playerId, int targetPlayerId, const std::string& targetCardName) override;
};

class ThreeIdenticalCatsRule : public SpecialRule {
public:
    ThreeIdenticalCatsRule()
        : SpecialRule("แมวเหมือนกัน 3 ใบ", 3)
    {
    }

    bool checkCondition(const Player& player) const override
    {
        return player.firstCatWithCount(requiredCardCount).has_value();
    }

    void executeAction(GameManager& gm, int playerId, int targetPlayerId, const std::string& targetCardName) override;
};

class FiveDifferentCatsRule : public SpecialRule {
public:
    FiveDifferentCatsRule()
        : SpecialRule("แมวต่างกัน 5 ใบ", 5)
    {
    }

    bool checkCondition(const Player& player) const override
    {
        return player.differentCatCount() >= requiredCardCount;
    }

    void executeAction(GameManager& gm, int playerId, int targetPlayerId, const std::string& targetCardName) override;
};

enum class GamePhase {
    PassDevice,
    Playing,
    ChooseTarget,
    ChooseCardName,
    ChooseDiscardCard,
    ChooseNopeResponse,
    FutureView,
    ChooseBoomPosition,
    GameOver
};

enum class PendingAction {
    None,
    Favor,
    TwoCats,
    ThreeCats
};

class GameManager {
public:
    static GameManager& instance()
    {
        static GameManager gm;
        return gm;
    }

    void startGame(const std::vector<std::string>& playerNames = {})
    {
        NetworkManager::instance().create_room();

        players.clear();
        deck.clear();
        discardPile.clear();
        logs_.clear();
        selectedCards.clear();
        selectedCard.clear();
        pendingBoom_.reset();
        currentPlayerIndex = 0;
        pendingAction_ = PendingAction::None;
        pendingTarget_ = -1;
        pendingCardIndex_ = -1;
        nopeResponder_ = -1;
        deck_count = 0;
        discardPile_count = 0;
        is_skipping_draw = false;
        winnerIndex_ = -1;

        int configuredPlayerCount = playerNames.empty() ? 4 : static_cast<int>(playerNames.size());
        configuredPlayerCount = std::clamp(configuredPlayerCount, 2, 4);
        for (int i = 0; i < configuredPlayerCount; ++i) {
            std::string fallbackName = "ผู้เล่น " + std::to_string(i + 1);
            std::string configuredName = i < static_cast<int>(playerNames.size()) ? playerNames[i] : fallbackName;
            players.emplace_back(configuredName.empty() ? fallbackName : configuredName);
        }

        std::vector<std::unique_ptr<Card>> dealPile;
        auto addCards = [&dealPile](CardType type, int quantity) {
            for (int i = 0; i < quantity; ++i) {
                dealPile.push_back(createCard(type));
            }
        };

        addCards(CardType::Skip, 4);
        addCards(CardType::Favor, 4);
        addCards(CardType::DrawFromBottom, 3);
        addCards(CardType::SeeTheFuture, 5);
        addCards(CardType::Shuffle, 4);
        addCards(CardType::Nope, 5);
        addCards(CardType::Defuse, 2);

        for (CatType type : allCatTypes()) {
            for (int i = 0; i < 4; ++i) {
                dealPile.push_back(createCard(CardType::CatCard, type));
            }
        }

        std::shuffle(dealPile.begin(), dealPile.end(), rng_);

        for (auto& player : players) {
            player.draw(createCard(CardType::Defuse));
            for (int i = 0; i < 5 && !dealPile.empty(); ++i) {
                player.draw(std::move(dealPile.back()));
                dealPile.pop_back();
            }
        }

        while (!dealPile.empty()) {
            deck.push_back(std::move(dealPile.back()));
            dealPile.pop_back();
        }

        for (int i = 0; i < static_cast<int>(players.size()) - 1; ++i) {
            deck.push_back(createCard(CardType::Boom));
        }
        shuffle_deck();

        specialRules.clear();
        specialRules.push_back(std::make_unique<TwoIdenticalCatsRule>());
        specialRules.push_back(std::make_unique<ThreeIdenticalCatsRule>());
        specialRules.push_back(std::make_unique<FiveDifferentCatsRule>());

        for (auto& player : players) {
            player.isTurn = false;
        }
        players[currentPlayerIndex].isTurn = true;
        phase_ = GamePhase::PassDevice;
        banner_ = "ส่งเครื่องให้ " + players[currentPlayerIndex].name;
        addLog("เริ่มเกมสำหรับผู้เล่น " + std::to_string(players.size()) + " คน");
        addLog(players[currentPlayerIndex].name + " เริ่มก่อน");
        syncCounts();
    }

    void EndGame()
    {
        winnerIndex_ = findWinner();
        phase_ = GamePhase::GameOver;
        if (winnerIndex_ >= 0) {
            banner_ = "ผู้ชนะคือ " + players[winnerIndex_].name;
            addLog(players[winnerIndex_].name + " เป็นผู้รอดชีวิตคนสุดท้าย");
        } else {
            banner_ = "จบเกม";
        }
    }

    void gameLoop() {}

    std::unique_ptr<Card> randomCard()
    {
        std::vector<CardType> types = {
            CardType::Skip,
            CardType::Favor,
            CardType::DrawFromBottom,
            CardType::SeeTheFuture,
            CardType::Shuffle,
            CardType::Nope,
            CardType::Defuse
        };
        std::uniform_int_distribution<int> typeDist(0, static_cast<int>(types.size()) - 1);
        CardType chosen = types[typeDist(rng_)];
        if (chosen == CardType::CatCard) {
            std::uniform_int_distribution<int> catDist(0, 4);
            return createCard(CardType::CatCard, allCatTypes()[catDist(rng_)]);
        }
        return createCard(chosen);
    }

    void shuffle_deck()
    {
        std::shuffle(deck.begin(), deck.end(), rng_);
        addLog("สับกองจั่วแล้ว");
    }

    void draw_card(int player_id, bool fromBottom = false)
    {
        if (!isValidPlayer(player_id) || deck.empty() || phase_ == GamePhase::GameOver) {
            return;
        }

        auto drawn = drawFromDeck(fromBottom);
        if (!drawn) {
            return;
        }

        const std::string drawnName = drawn->name();
        if (drawn->type() == CardType::Boom) {
            addLog(players[player_id].name + " จั่วได้ระเบิด");
            resolveBoom(player_id, std::move(drawn));
            return;
        }

        players[player_id].draw(std::move(drawn));
        addLog(players[player_id].name + " จั่วได้ " + drawnName);
        endTurn();
    }

    void drawCurrentPlayer(bool fromBottom)
    {
        if (phase_ != GamePhase::Playing) {
            return;
        }
        draw_card(currentPlayerIndex, fromBottom);
    }

    void add_card_to_deck(std::unique_ptr<Card> card, int position)
    {
        if (!card) {
            return;
        }
        position = std::max(0, std::min(position, static_cast<int>(deck.size())));
        deck.insert(deck.begin() + position, std::move(card));
        syncCounts();
    }

    void add_card_to_deck(Card* card, int position)
    {
        if (card) {
            add_card_to_deck(card->clone(), position);
        }
    }

    void processSpecialRule(int player_id, int target_id, const std::string& cardName)
    {
        if (!isValidPlayer(player_id)) {
            return;
        }

        switch (pendingAction_) {
        case PendingAction::TwoCats:
            specialRules[0]->executeAction(*this, player_id, target_id, cardName);
            break;
        case PendingAction::ThreeCats:
            specialRules[1]->executeAction(*this, player_id, target_id, cardName);
            break;
        case PendingAction::None:
        case PendingAction::Favor:
            break;
        }
        pendingAction_ = PendingAction::None;
        pendingTarget_ = -1;
        selectedCard.clear();
        phase_ = GamePhase::Playing;
        banner_ = "ตาของ " + players[currentPlayerIndex].name;
        syncCounts();
    }

    bool playCard(int cardIndex)
    {
        if (phase_ != GamePhase::Playing || !isValidPlayer(currentPlayerIndex)) {
            return false;
        }
        Player& player = players[currentPlayerIndex];
        if (cardIndex < 0 || cardIndex >= static_cast<int>(player.myCard.size())) {
            return false;
        }

        CardType type = player.myCard[cardIndex]->type();
        if (type == CardType::Defuse) {
            addLog("เก็บการ์ดกู้ระเบิดไว้ในมือ การ์ดนี้จะใช้เองเมื่อจั่วเจอระเบิด");
            return false;
        }
        if (type == CardType::Boom) {
            addLog("ไม่สามารถเล่นการ์ดระเบิดจากมือได้");
            return false;
        }
        if (type == CardType::CatCard) {
            addLog("ใช้ปุ่มคอมโบแมวสำหรับการ์ดแมว");
            return false;
        }
        if (type == CardType::Nope) {
            addLog("การ์ดยกเลิกจะใช้ได้เมื่อระบบถามเท่านั้น");
            return false;
        }

        if (type == CardType::Favor) {
            pendingAction_ = PendingAction::Favor;
            pendingCardIndex_ = cardIndex;
            phase_ = GamePhase::ChooseTarget;
            banner_ = "เลือกผู้เล่นสำหรับการ์ดขอความช่วยเหลือ";
            return true;
        }

        auto card = player.removeCardAt(cardIndex);
        const std::string cardName = card->name();
        discard(std::move(card));
        NetworkManager::instance().rpc_send_card_action(cardIndex);
        addLog(player.name + " เล่นการ์ด " + cardName);
        discardPile.back()->OnUse(*this);
        syncCounts();
        return true;
    }

    void revealCurrentHand()
    {
        if (phase_ == GamePhase::PassDevice) {
            phase_ = GamePhase::Playing;
            banner_ = "ตาของ " + players[currentPlayerIndex].name;
        }
    }

    void requestTwoCatsRule()
    {
        if (phase_ != GamePhase::Playing) {
            return;
        }
        if (!specialRules[0]->checkCondition(players[currentPlayerIndex])) {
            addLog("ต้องมีการ์ดแมวชนิดเดียวกัน 2 ใบ");
            return;
        }
        pendingAction_ = PendingAction::TwoCats;
        phase_ = GamePhase::ChooseTarget;
        banner_ = "เลือกผู้เล่นสำหรับคอมโบแมว 2 ใบ";
    }

    void requestThreeCatsRule()
    {
        if (phase_ != GamePhase::Playing) {
            return;
        }
        if (!specialRules[1]->checkCondition(players[currentPlayerIndex])) {
            addLog("ต้องมีการ์ดแมวชนิดเดียวกัน 3 ใบ");
            return;
        }
        pendingAction_ = PendingAction::ThreeCats;
        phase_ = GamePhase::ChooseTarget;
        banner_ = "เลือกผู้เล่นสำหรับคอมโบแมว 3 ใบ";
    }

    void requestFiveCatsRule()
    {
        if (phase_ != GamePhase::Playing) {
            return;
        }
        if (!specialRules[2]->checkCondition(players[currentPlayerIndex])) {
            addLog("ต้องมีการ์ดแมวต่างชนิดกัน 5 ใบ");
            return;
        }
        if (discardPile.empty()) {
            addLog("กองทิ้งยังว่างอยู่");
            return;
        }
        pendingAction_ = PendingAction::None;
        phase_ = GamePhase::ChooseDiscardCard;
        banner_ = "เลือกการ์ดหนึ่งใบจากกองทิ้ง";
    }

    void chooseTarget(int targetId)
    {
        if (phase_ != GamePhase::ChooseTarget || !isValidPlayer(targetId) || targetId == currentPlayerIndex || !players[targetId].isAlive) {
            return;
        }

        pendingTarget_ = targetId;

        if (pendingAction_ == PendingAction::ThreeCats) {
            phase_ = GamePhase::ChooseCardName;
            banner_ = "เลือกชื่อการ์ดที่ต้องการขโมย";
            return;
        }

        commitPendingActionBeforeNope("");
    }

    void chooseNamedCard(const std::string& cardName)
    {
        if (phase_ != GamePhase::ChooseCardName) {
            return;
        }
        selectedCard = cardName;
        commitPendingActionBeforeNope(cardName);
    }

    void chooseDiscardCard(int discardIndex)
    {
        if (phase_ != GamePhase::ChooseDiscardCard || discardIndex < 0 || discardIndex >= static_cast<int>(discardPile.size())) {
            return;
        }
        std::string cardName = discardPile[discardIndex]->name();
        specialRules[2]->executeAction(*this, currentPlayerIndex, currentPlayerIndex, cardName);
        phase_ = GamePhase::Playing;
        banner_ = "ตาของ " + players[currentPlayerIndex].name;
        syncCounts();
    }

    void respondNope(bool useNope)
    {
        if (phase_ != GamePhase::ChooseNopeResponse || !isValidPlayer(nopeResponder_)) {
            return;
        }

        if (useNope && players[nopeResponder_].hasCard(CardType::Nope)) {
            int nopeIndex = players[nopeResponder_].findFirst(CardType::Nope);
            discard(players[nopeResponder_].removeCardAt(nopeIndex));
            addLog(players[nopeResponder_].name + " เล่นการ์ดยกเลิก การกระทำถูกยกเลิก");
            pendingAction_ = PendingAction::None;
            pendingTarget_ = -1;
            selectedCard.clear();
            phase_ = GamePhase::Playing;
            banner_ = "ตาของ " + players[currentPlayerIndex].name;
            syncCounts();
            return;
        }

        addLog(players[nopeResponder_].name + " ไม่ใช้การ์ดยกเลิก");
        executePendingAction();
    }

    void closeFutureView()
    {
        if (phase_ == GamePhase::FutureView) {
            phase_ = GamePhase::Playing;
            banner_ = "ตาของ " + players[currentPlayerIndex].name;
            futureCards_.clear();
        }
    }

    void placePendingBoom(int position)
    {
        if (phase_ != GamePhase::ChooseBoomPosition || !pendingBoom_) {
            return;
        }

        int insertPos = 0;
        if (position == 0) {
            insertPos = static_cast<int>(deck.size());
            addLog("วางระเบิดไว้ใกล้บนสุดของกอง");
        } else if (position == 1) {
            insertPos = static_cast<int>(deck.size() / 2);
            addLog("วางระเบิดไว้กลางกอง");
        } else {
            insertPos = 0;
            addLog("วางระเบิดไว้ล่างสุดของกอง");
        }
        add_card_to_deck(std::move(pendingBoom_), insertPos);
        endTurn();
    }

    void skipCurrentTurn()
    {
        is_skipping_draw = true;
        addLog(players[currentPlayerIndex].name + " ข้ามการจั่ว");
        endTurn();
    }

    void startFavorSelection()
    {
        phase_ = GamePhase::ChooseTarget;
        banner_ = "เลือกผู้เล่นสำหรับการ์ดขอความช่วยเหลือ";
    }

    void revealFuture(int amount)
    {
        futureCards_.clear();
        amount = std::min(amount, static_cast<int>(deck.size()));
        for (int i = 0; i < amount; ++i) {
            const auto& card = deck[deck.size() - 1 - i];
            futureCards_.push_back(card->name());
        }
        if (futureCards_.empty()) {
            futureCards_.push_back("กองจั่วว่างแล้ว");
        }
        phase_ = GamePhase::FutureView;
        banner_ = "การ์ดบนสุดของกอง";
    }

    void useDrawFromBottom()
    {
        draw_card(currentPlayerIndex, true);
    }

    void addLog(const std::string& text)
    {
        logs_.push_back(text);
        if (logs_.size() > 12) {
            logs_.erase(logs_.begin());
        }
    }

    void discard(std::unique_ptr<Card> card)
    {
        if (card) {
            discardPile.push_back(std::move(card));
            syncCounts();
        }
    }

    bool isValidPlayer(int playerId) const
    {
        return playerId >= 0 && playerId < static_cast<int>(players.size());
    }

    bool discardCatsFromPlayer(int playerId, CatType type, int count)
    {
        if (!isValidPlayer(playerId)) {
            return false;
        }
        auto indices = players[playerId].catIndices(type, count);
        if (static_cast<int>(indices.size()) < count) {
            return false;
        }
        std::sort(indices.rbegin(), indices.rend());
        for (int index : indices) {
            discard(players[playerId].removeCardAt(index));
        }
        return true;
    }

    bool discardOneOfEachCatFromPlayer(int playerId)
    {
        if (!isValidPlayer(playerId)) {
            return false;
        }
        auto indices = players[playerId].oneOfEachCatIndices();
        if (indices.size() < allCatTypes().size()) {
            return false;
        }
        std::sort(indices.rbegin(), indices.rend());
        for (int index : indices) {
            discard(players[playerId].removeCardAt(index));
        }
        return true;
    }

    void stealRandomCard(int playerId, int targetPlayerId)
    {
        if (!isValidPlayer(playerId) || !isValidPlayer(targetPlayerId)) {
            return;
        }
        auto stolen = players[targetPlayerId].takeRandomCard(rng_);
        if (!stolen) {
            addLog(players[targetPlayerId].name + " ไม่มีการ์ดให้ขโมย");
            return;
        }
        std::string cardName = stolen->name();
        players[playerId].draw(std::move(stolen));
        addLog(players[playerId].name + " ขโมยการ์ดสุ่มได้ " + cardName);
    }

    void stealNamedCard(int playerId, int targetPlayerId, const std::string& cardName)
    {
        if (!isValidPlayer(playerId) || !isValidPlayer(targetPlayerId)) {
            return;
        }
        auto stolen = players[targetPlayerId].takeNamedCard(cardName);
        if (!stolen) {
            addLog(players[targetPlayerId].name + " ไม่มีการ์ด " + cardName);
            return;
        }
        players[playerId].draw(std::move(stolen));
        addLog(players[playerId].name + " ขโมยการ์ด " + cardName);
    }

    bool takeFromDiscardByName(int playerId, const std::string& cardName)
    {
        if (!isValidPlayer(playerId)) {
            return false;
        }
        for (int i = static_cast<int>(discardPile.size()) - 1; i >= 0; --i) {
            if (discardPile[i]->name() == cardName) {
                auto card = std::move(discardPile[i]);
                discardPile.erase(discardPile.begin() + i);
                players[playerId].draw(std::move(card));
                addLog(players[playerId].name + " หยิบ " + cardName + " จากกองทิ้ง");
                syncCounts();
                return true;
            }
        }
        addLog(cardName + " ไม่อยู่ในกองทิ้งแล้ว");
        return false;
    }

    bool hasNope(int playerId) const
    {
        return isValidPlayer(playerId) && players[playerId].hasCard(CardType::Nope);
    }

    const std::vector<std::string>& logs() const { return logs_; }
    const std::vector<std::string>& futureCards() const { return futureCards_; }
    GamePhase phase() const { return phase_; }
    PendingAction pendingAction() const { return pendingAction_; }
    const std::string& banner() const { return banner_; }
    int pendingTarget() const { return pendingTarget_; }
    int nopeResponder() const { return nopeResponder_; }
    int winnerIndex() const { return winnerIndex_; }

    std::vector<Player> players;
    std::vector<std::unique_ptr<Card>> deck;
    int deck_count = 0;
    int currentPlayerIndex = 0;
    std::vector<std::unique_ptr<Card>> discardPile;
    int discardPile_count = 0;
    int t_deck_count = 0;
    bool is_skipping_draw = false;
    std::vector<std::unique_ptr<SpecialRule>> specialRules;
    std::vector<Card*> selectedCards;
    std::string selectedCard;

private:
    GameManager()
        : rng_(std::random_device{}())
    {
    }

    std::unique_ptr<Card> drawFromDeck(bool fromBottom)
    {
        if (deck.empty()) {
            EndGame();
            return nullptr;
        }

        std::unique_ptr<Card> card;
        if (fromBottom) {
            card = std::move(deck.front());
            deck.erase(deck.begin());
        } else {
            card = std::move(deck.back());
            deck.pop_back();
        }
        syncCounts();
        return card;
    }

    void resolveBoom(int playerId, std::unique_ptr<Card> boomCard)
    {
        int defuseIndex = players[playerId].findFirst(CardType::Defuse);
        if (defuseIndex >= 0) {
            discard(players[playerId].removeCardAt(defuseIndex));
            pendingBoom_ = std::move(boomCard);
            phase_ = GamePhase::ChooseBoomPosition;
            banner_ = players[playerId].name + " กู้ระเบิดสำเร็จ";
            addLog(players[playerId].name + " ใช้การ์ดกู้ระเบิด");
            return;
        }

        discard(std::move(boomCard));
        players[playerId].isAlive = false;
        players[playerId].isActive = false;
        addLog(players[playerId].name + " ระเบิดและออกจากเกม");
        if (activePlayerCount() <= 1) {
            EndGame();
            return;
        }
        endTurn();
    }

    void endTurn()
    {
        if (phase_ == GamePhase::GameOver) {
            return;
        }
        is_skipping_draw = false;
        players[currentPlayerIndex].isTurn = false;
        advanceToNextAlive();
        players[currentPlayerIndex].isTurn = true;
        phase_ = GamePhase::PassDevice;
        banner_ = "ส่งเครื่องให้ " + players[currentPlayerIndex].name;
        addLog("เปลี่ยนตาเป็น " + players[currentPlayerIndex].name);
        syncCounts();
    }

    void advanceToNextAlive()
    {
        if (players.empty()) {
            return;
        }
        for (int i = 0; i < static_cast<int>(players.size()); ++i) {
            currentPlayerIndex = (currentPlayerIndex + 1) % static_cast<int>(players.size());
            if (players[currentPlayerIndex].isAlive) {
                return;
            }
        }
    }

    int activePlayerCount() const
    {
        return static_cast<int>(std::count_if(players.begin(), players.end(), [](const Player& player) {
            return player.isAlive;
        }));
    }

    int findWinner() const
    {
        for (int i = 0; i < static_cast<int>(players.size()); ++i) {
            if (players[i].isAlive) {
                return i;
            }
        }
        return -1;
    }

    void syncCounts()
    {
        deck_count = static_cast<int>(deck.size());
        discardPile_count = static_cast<int>(discardPile.size());
        t_deck_count = deck_count;
    }

    void commitPendingActionBeforeNope(const std::string& cardName)
    {
        if (pendingAction_ == PendingAction::Favor) {
            if (pendingCardIndex_ < 0 || pendingCardIndex_ >= static_cast<int>(players[currentPlayerIndex].myCard.size())) {
                pendingAction_ = PendingAction::None;
                phase_ = GamePhase::Playing;
                return;
            }
            auto card = players[currentPlayerIndex].removeCardAt(pendingCardIndex_);
            addLog(players[currentPlayerIndex].name + " เล่นการ์ดขอความช่วยเหลือ");
            discard(std::move(card));
        } else if (pendingAction_ == PendingAction::TwoCats) {
            auto catType = players[currentPlayerIndex].firstCatWithCount(2);
            if (!catType.has_value() || !discardCatsFromPlayer(currentPlayerIndex, catType.value(), 2)) {
                addLog("คอมโบแมว 2 ใบไม่สำเร็จ");
                pendingAction_ = PendingAction::None;
                phase_ = GamePhase::Playing;
                return;
            }
            addLog(players[currentPlayerIndex].name + " เล่นคอมโบแมวเหมือนกัน 2 ใบ");
        } else if (pendingAction_ == PendingAction::ThreeCats) {
            auto catType = players[currentPlayerIndex].firstCatWithCount(3);
            if (!catType.has_value() || !discardCatsFromPlayer(currentPlayerIndex, catType.value(), 3)) {
                addLog("คอมโบแมว 3 ใบไม่สำเร็จ");
                pendingAction_ = PendingAction::None;
                phase_ = GamePhase::Playing;
                return;
            }
            addLog(players[currentPlayerIndex].name + " เล่นคอมโบแมวเหมือนกัน 3 ใบ");
            selectedCard = cardName;
        }

        if (hasNope(pendingTarget_)) {
            nopeResponder_ = pendingTarget_;
            phase_ = GamePhase::ChooseNopeResponse;
            banner_ = players[nopeResponder_].name + " สามารถใช้การ์ดยกเลิกได้";
            return;
        }

        executePendingAction();
    }

    void executePendingAction()
    {
        if (pendingAction_ == PendingAction::Favor) {
            stealRandomCard(currentPlayerIndex, pendingTarget_);
        } else if (pendingAction_ == PendingAction::TwoCats || pendingAction_ == PendingAction::ThreeCats) {
            processSpecialRule(currentPlayerIndex, pendingTarget_, selectedCard);
            return;
        }
        pendingAction_ = PendingAction::None;
        pendingTarget_ = -1;
        selectedCard.clear();
        phase_ = GamePhase::Playing;
        banner_ = "ตาของ " + players[currentPlayerIndex].name;
        syncCounts();
    }

    std::mt19937 rng_;
    std::vector<std::string> logs_;
    std::vector<std::string> futureCards_;
    GamePhase phase_ = GamePhase::PassDevice;
    PendingAction pendingAction_ = PendingAction::None;
    std::string banner_;
    int pendingTarget_ = -1;
    int pendingCardIndex_ = -1;
    int nopeResponder_ = -1;
    int winnerIndex_ = -1;
    std::unique_ptr<Card> pendingBoom_;
};

inline void TwoIdenticalCatsRule::executeAction(GameManager& gm, int playerId, int targetPlayerId, const std::string&)
{
    gm.stealRandomCard(playerId, targetPlayerId);
}

inline void ThreeIdenticalCatsRule::executeAction(GameManager& gm, int playerId, int targetPlayerId, const std::string& targetCardName)
{
    if (targetCardName.empty()) {
        gm.addLog("ยังไม่ได้เลือกชื่อการ์ด");
        return;
    }
    gm.stealNamedCard(playerId, targetPlayerId, targetCardName);
}

inline void FiveDifferentCatsRule::executeAction(GameManager& gm, int playerId, int, const std::string& targetCardName)
{
    if (targetCardName.empty()) {
        gm.addLog("เลือกการ์ดจากกองทิ้งก่อน");
        return;
    }

    if (!gm.takeFromDiscardByName(playerId, targetCardName)) {
        return;
    }
    if (!gm.discardOneOfEachCatFromPlayer(playerId)) {
        gm.addLog("คอมโบแมวต่างกัน 5 ใบไม่สำเร็จ");
        return;
    }
    gm.addLog(gm.players[playerId].name + " ใช้คอมโบแมวต่างกัน 5 ใบ");
}

inline void Boom::OnUse(GameManager& gm)
{
    gm.addLog("การ์ดระเบิดรออยู่ในกองจั่ว");
}

inline void Defuse::OnUse(GameManager& gm)
{
    gm.addLog("การ์ดกู้ระเบิดจะใช้เองเมื่อจั่วเจอระเบิด");
}

inline void Skip::OnUse(GameManager& gm)
{
    gm.skipCurrentTurn();
}

inline void Favor::OnUse(GameManager& gm)
{
    gm.startFavorSelection();
}

inline void SeeTheFuture::OnUse(GameManager& gm)
{
    gm.revealFuture(3);
}

inline void Shuffle::OnUse(GameManager& gm)
{
    gm.shuffle_deck();
}

inline void Nope::OnUse(GameManager& gm)
{
    gm.addLog("การ์ดยกเลิกจะใช้ได้เมื่อหน้าต่างตอบสนองปรากฏ");
}

inline void DrawFromBottom::OnUse(GameManager& gm)
{
    gm.useDrawFromBottom();
}

inline void CatCard::OnUse(GameManager& gm)
{
    gm.addLog("การ์ดแมวใช้ผ่านกติกาคอมโบ");
}
