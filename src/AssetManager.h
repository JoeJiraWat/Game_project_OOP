#pragma once

#include "raylib.h"

#include <array>

class Card;
enum class CardType;

class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager();

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    void load();
    void unload();
    void updateMusic();

    const Texture2D* background() const;
    const Texture2D* textureForCard(const Card& card) const;
    const Font* font() const;
    bool hasMusic() const { return musicLoaded_; }

private:
    Texture2D background_ {};
    Texture2D bomb_ {};
    Texture2D defuse_ {};
    Texture2D skip_ {};
    Texture2D favor_ {};
    Texture2D shuffle_ {};
    Texture2D nope_ {};
    Texture2D drawFromBottom_ {};
    Texture2D seeTheFuture_ {};
    std::array<Texture2D, 5> cats_ {};
    Font font_ {};
    Music music_ {};

    bool backgroundLoaded_ = false;
    bool bombLoaded_ = false;
    bool defuseLoaded_ = false;
    bool skipLoaded_ = false;
    bool favorLoaded_ = false;
    bool shuffleLoaded_ = false;
    bool nopeLoaded_ = false;
    bool drawFromBottomLoaded_ = false;
    bool seeTheFutureLoaded_ = false;
    std::array<bool, 5> catsLoaded_ {};
    bool fontLoaded_ = false;
    bool musicLoaded_ = false;
};
