#include "AssetManager.h"

#include "GameModel.h"

#include <initializer_list>
#include <string>
#include <vector>

namespace {

std::string assetPath(const char* path)
{
    if (FileExists(path)) {
        return path;
    }
    std::string parentPath = std::string("../") + path;
    if (FileExists(parentPath.c_str())) {
        return parentPath;
    }
    return path;
}

bool loadTexture(Texture2D& texture, const char* path)
{
    std::string resolvedPath = assetPath(path);
    if (!FileExists(resolvedPath.c_str())) {
        return false;
    }
    texture = LoadTexture(resolvedPath.c_str());
    return IsTextureValid(texture);
}

bool loadTextureAny(Texture2D& texture, std::initializer_list<const char*> paths)
{
    for (const char* path : paths) {
        if (loadTexture(texture, path)) {
            return true;
        }
    }
    return false;
}

void unloadTexture(Texture2D& texture, bool& loaded)
{
    if (loaded) {
        UnloadTexture(texture);
        texture = {};
        loaded = false;
    }
}

bool loadReadableFont(Font& font)
{
    const char* candidates[] = {
        "Fonts/ArialUnicode.ttf",
        "Fonts/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Artifakt Element Regular.ttf"
    };

    std::vector<int> codepoints;
    codepoints.reserve(256);
    for (int cp = 32; cp <= 126; ++cp) {
        codepoints.push_back(cp);
    }
    for (int cp = 0x0E00; cp <= 0x0E7F; ++cp) {
        codepoints.push_back(cp);
    }

    for (const char* path : candidates) {
        if (!FileExists(path)) {
            continue;
        }
        font = LoadFontEx(path, 64, codepoints.data(), static_cast<int>(codepoints.size()));
        if (IsFontValid(font)) {
            SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
            return true;
        }
    }

    return false;
}

} // namespace

AssetManager::~AssetManager()
{
    unload();
}

void AssetManager::load()
{
    unload();

    backgroundLoaded_ = loadTexture(background_, "Background/StartUIGame.png");
    bombLoaded_ = loadTexture(bomb_, "Card/bomb.png");
    defuseLoaded_ = loadTexture(defuse_, "Card/defuse.png");
    skipLoaded_ = loadTexture(skip_, "Card/skip.png");
    favorLoaded_ = loadTextureAny(favor_, { "Card/favor.png", "Card/flover.png" });
    shuffleLoaded_ = loadTexture(shuffle_, "Card/shuffle.png");
    nopeLoaded_ = loadTexture(nope_, "Card/nope.png");
    drawFromBottomLoaded_ = loadTextureAny(drawFromBottom_, { "Card/draw_from_bottom.png", "Card/draw_from_the_bottom.png", "Card/draw_from_the_buttom.png", "Card/drawfrombottom.png" });
    seeTheFutureLoaded_ = loadTextureAny(seeTheFuture_, { "Card/see_the_future.png", "Card/seethefuture.png" });
    catsLoaded_[0] = loadTexture(cats_[0], "Card/cat1.png");
    catsLoaded_[1] = loadTexture(cats_[1], "Card/cat2.png");
    catsLoaded_[2] = loadTexture(cats_[2], "Card/cat3.png");
    catsLoaded_[3] = loadTexture(cats_[3], "Card/cat4.png");
    catsLoaded_[4] = loadTexture(cats_[4], "Card/cat5.png");
    fontLoaded_ = loadReadableFont(font_);

    std::string musicPath = assetPath("Sound/Scorch_Logic.mp3");
    if (FileExists(musicPath.c_str())) {
        music_ = LoadMusicStream(musicPath.c_str());
        musicLoaded_ = IsMusicValid(music_);
        if (musicLoaded_) {
            SetMusicVolume(music_, 0.38f);
            PlayMusicStream(music_);
        }
    }
}

void AssetManager::unload()
{
    if (musicLoaded_) {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
        music_ = {};
        musicLoaded_ = false;
    }

    unloadTexture(background_, backgroundLoaded_);
    unloadTexture(bomb_, bombLoaded_);
    unloadTexture(defuse_, defuseLoaded_);
    unloadTexture(skip_, skipLoaded_);
    unloadTexture(favor_, favorLoaded_);
    unloadTexture(shuffle_, shuffleLoaded_);
    unloadTexture(nope_, nopeLoaded_);
    unloadTexture(drawFromBottom_, drawFromBottomLoaded_);
    unloadTexture(seeTheFuture_, seeTheFutureLoaded_);
    for (int i = 0; i < static_cast<int>(cats_.size()); ++i) {
        unloadTexture(cats_[i], catsLoaded_[i]);
    }

    if (fontLoaded_) {
        UnloadFont(font_);
        font_ = {};
        fontLoaded_ = false;
    }
}

void AssetManager::updateMusic()
{
    if (musicLoaded_) {
        UpdateMusicStream(music_);
    }
}

const Texture2D* AssetManager::background() const
{
    return backgroundLoaded_ ? &background_ : nullptr;
}

const Texture2D* AssetManager::textureForCard(const Card& card) const
{
    switch (card.type()) {
    case CardType::Boom:
        return bombLoaded_ ? &bomb_ : nullptr;
    case CardType::Defuse:
        return defuseLoaded_ ? &defuse_ : nullptr;
    case CardType::Skip:
        return skipLoaded_ ? &skip_ : nullptr;
    case CardType::Favor:
        return favorLoaded_ ? &favor_ : nullptr;
    case CardType::Shuffle:
        return shuffleLoaded_ ? &shuffle_ : nullptr;
    case CardType::Nope:
        return nopeLoaded_ ? &nope_ : nullptr;
    case CardType::DrawFromBottom:
        return drawFromBottomLoaded_ ? &drawFromBottom_ : nullptr;
    case CardType::SeeTheFuture:
        return seeTheFutureLoaded_ ? &seeTheFuture_ : nullptr;
    case CardType::CatCard:
        if (card.catType().has_value()) {
            int index = static_cast<int>(card.catType().value());
            if (index >= 0 && index < static_cast<int>(cats_.size()) && catsLoaded_[index]) {
                return &cats_[index];
            }
        }
        return nullptr;
    }
    return nullptr;
}

const Font* AssetManager::font() const
{
    return fontLoaded_ ? &font_ : nullptr;
}
