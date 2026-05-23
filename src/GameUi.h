#pragma once

#include "AssetManager.h"

#include <array>
#include <string>

class CatBombApp {
public:
    CatBombApp();
    ~CatBombApp() = default;

    void updateAndDraw();

private:
    enum class Screen {
        Start,
        Setup,
        Game
    };

    void drawStartScreen();
    void drawSetupScreen();
    void drawGameScreen();
    void drawCardHelpModal();
    void drawBackgroundTint();
    void handleNameInput();
    void startConfiguredGame();

    AssetManager assets_;
    Screen screen_ = Screen::Start;
    int playerCount_ = 4;
    int activeNameIndex_ = 0;
    bool showCardHelp_ = false;
    std::array<std::string, 4> playerNames_ {
        "Player 1",
        "Player 2",
        "Player 3",
        "Player 4"
    };
};
