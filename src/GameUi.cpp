#include "GameUi.h"

#include "GameModel.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

namespace {

const Font* gUiFont = nullptr;
constexpr float kFontSpacing = 1.0f;

static void setUiFont(const Font* font)
{
    gUiFont = font;
}

static int MeasureUiText(const char* text, int fontSize)
{
    if (gUiFont != nullptr && text != nullptr) {
        return static_cast<int>(std::ceil(::MeasureTextEx(*gUiFont, text, static_cast<float>(fontSize), kFontSpacing).x));
    }
    return ::MeasureText(text, fontSize);
}

static void DrawUiText(const char* text, int posX, int posY, int fontSize, Color color)
{
    if (gUiFont != nullptr && text != nullptr) {
        ::DrawTextEx(*gUiFont, text, Vector2 { static_cast<float>(posX), static_cast<float>(posY) }, static_cast<float>(fontSize), kFontSpacing, color);
        return;
    }
    ::DrawText(text, posX, posY, fontSize, color);
}

static Color colorForCard(const Card& card)
{
    if (card.type() == CardType::CatCard && card.catType().has_value()) {
        switch (card.catType().value()) {
        case CatType::Cat1:
            return Color { 247, 184, 70, 255 };
        case CatType::Cat2:
            return Color { 96, 197, 184, 255 };
        case CatType::Cat3:
            return Color { 241, 112, 112, 255 };
        case CatType::Cat4:
            return Color { 150, 190, 98, 255 };
        case CatType::Cat5:
            return Color { 170, 126, 207, 255 };
        }
    }

    switch (card.type()) {
    case CardType::Boom:
        return Color { 218, 65, 70, 255 };
    case CardType::Defuse:
        return Color { 72, 181, 116, 255 };
    case CardType::Skip:
        return Color { 87, 153, 220, 255 };
    case CardType::Favor:
        return Color { 240, 149, 65, 255 };
    case CardType::DrawFromBottom:
        return Color { 104, 123, 194, 255 };
    case CardType::SeeTheFuture:
        return Color { 93, 183, 211, 255 };
    case CardType::Shuffle:
        return Color { 229, 196, 69, 255 };
    case CardType::Nope:
        return Color { 52, 47, 58, 255 };
    case CardType::CatCard:
        return Color { 247, 184, 70, 255 };
    }
    return WHITE;
}

static Color withAlpha(Color color, unsigned char alpha)
{
    color.a = alpha;
    return color;
}

static bool pointInRect(Vector2 point, Rectangle rect)
{
    return point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y && point.y <= rect.y + rect.height;
}

static void drawTextureCover(const Texture2D& texture, Rectangle dest, Color tint = WHITE)
{
    float scale = std::max(dest.width / static_cast<float>(texture.width), dest.height / static_cast<float>(texture.height));
    float sourceWidth = dest.width / scale;
    float sourceHeight = dest.height / scale;
    Rectangle source {
        (texture.width - sourceWidth) * 0.5f,
        (texture.height - sourceHeight) * 0.5f,
        sourceWidth,
        sourceHeight
    };
    DrawTexturePro(texture, source, dest, Vector2 { 0, 0 }, 0.0f, tint);
}

static void drawTextureFit(const Texture2D& texture, Rectangle dest, Color tint = WHITE)
{
    float scale = std::min(dest.width / static_cast<float>(texture.width), dest.height / static_cast<float>(texture.height));
    float width = texture.width * scale;
    float height = texture.height * scale;
    Rectangle fitted {
        dest.x + (dest.width - width) * 0.5f,
        dest.y + (dest.height - height) * 0.5f,
        width,
        height
    };
    DrawTexturePro(texture, Rectangle { 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) }, fitted, Vector2 { 0, 0 }, 0.0f, tint);
}

static std::vector<std::string> wrapText(const std::string& text, float maxWidth, float fontSize)
{
    std::vector<std::string> lines;
    std::istringstream words(text);
    std::string word;
    std::string line;
    const int spacing = 1;

    while (words >> word) {
        std::string candidate = line.empty() ? word : line + " " + word;
        if (MeasureUiText(candidate.c_str(), static_cast<int>(fontSize)) <= maxWidth || line.empty()) {
            line = candidate;
        } else {
            lines.push_back(line);
            line = word;
        }
    }

    if (!line.empty()) {
        lines.push_back(line);
    }
    (void)spacing;
    return lines;
}

static void drawWrappedText(const std::string& text, float x, float y, float maxWidth, float fontSize, Color color, int maxLines = 4)
{
    auto lines = wrapText(text, maxWidth, fontSize);
    for (int i = 0; i < static_cast<int>(lines.size()) && i < maxLines; ++i) {
        DrawUiText(lines[i].c_str(), static_cast<int>(x), static_cast<int>(y + i * (fontSize + 4)), static_cast<int>(fontSize), color);
    }
}

static bool drawButton(Rectangle rect, const std::string& label, bool enabled = true)
{
    Vector2 mouse = GetMousePosition();
    bool hover = enabled && pointInRect(mouse, rect);
    Color fill = enabled ? (hover ? Color { 255, 226, 121, 255 } : Color { 244, 202, 82, 255 }) : Color { 98, 94, 90, 255 };
    Color border = enabled ? Color { 34, 32, 30, 255 } : Color { 68, 65, 62, 255 };
    Color text = enabled ? Color { 31, 28, 25, 255 } : Color { 160, 156, 150, 255 };

    DrawRectangleRounded(rect, 0.12f, 8, fill);
    DrawRectangleRoundedLinesEx(rect, 0.12f, 8, 2.0f, border);

    int fontSize = 18;
    while (fontSize > 10 && MeasureUiText(label.c_str(), fontSize) > rect.width - 18) {
        --fontSize;
    }
    int textWidth = MeasureUiText(label.c_str(), fontSize);
    DrawUiText(label.c_str(), static_cast<int>(rect.x + (rect.width - textWidth) / 2), static_cast<int>(rect.y + (rect.height - fontSize) / 2), fontSize, text);

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static bool drawHelpButton(float screenWidth)
{
    return drawButton(Rectangle { screenWidth - 58.0f, 12.0f, 38.0f, 38.0f }, "?");
}

static void drawPanel(Rectangle rect, Color color)
{
    DrawRectangleRounded(rect, 0.08f, 10, color);
    DrawRectangleRoundedLinesEx(rect, 0.08f, 10, 1.5f, Color { 77, 72, 66, 255 });
}

static void drawCardIcon(const Card& card, Rectangle rect)
{
    Vector2 center { rect.x + rect.width * 0.5f, rect.y + rect.height * 0.49f };
    float s = std::min(rect.width, rect.height) / 110.0f;
    Color ink = Color { 31, 28, 25, 255 };
    Color light = Color { 255, 245, 220, 255 };

    switch (card.type()) {
    case CardType::Boom:
        DrawCircleV(center, 24 * s, Color { 60, 50, 46, 255 });
        DrawCircleV(Vector2 { center.x - 9 * s, center.y - 9 * s }, 9 * s, Color { 255, 111, 78, 255 });
        DrawLineEx(Vector2 { center.x + 14 * s, center.y - 20 * s }, Vector2 { center.x + 28 * s, center.y - 37 * s }, 5 * s, ink);
        DrawCircleV(Vector2 { center.x + 34 * s, center.y - 43 * s }, 6 * s, Color { 255, 226, 90, 255 });
        break;
    case CardType::Defuse:
        DrawRectangleRounded(Rectangle { center.x - 26 * s, center.y - 23 * s, 52 * s, 46 * s }, 0.25f, 8, Color { 235, 250, 223, 255 });
        DrawRectangleRoundedLinesEx(Rectangle { center.x - 26 * s, center.y - 23 * s, 52 * s, 46 * s }, 0.25f, 8, 3 * s, ink);
        DrawLineEx(Vector2 { center.x - 14 * s, center.y + 2 * s }, Vector2 { center.x - 2 * s, center.y + 14 * s }, 5 * s, Color { 72, 181, 116, 255 });
        DrawLineEx(Vector2 { center.x - 2 * s, center.y + 14 * s }, Vector2 { center.x + 19 * s, center.y - 13 * s }, 5 * s, Color { 72, 181, 116, 255 });
        break;
    case CardType::Skip:
        DrawTriangle(Vector2 { center.x - 28 * s, center.y - 24 * s }, Vector2 { center.x - 28 * s, center.y + 24 * s }, Vector2 { center.x + 8 * s, center.y }, light);
        DrawTriangle(Vector2 { center.x + 3 * s, center.y - 24 * s }, Vector2 { center.x + 3 * s, center.y + 24 * s }, Vector2 { center.x + 39 * s, center.y }, light);
        break;
    case CardType::Favor:
        DrawCircleV(Vector2 { center.x - 14 * s, center.y - 4 * s }, 17 * s, Color { 255, 225, 185, 255 });
        DrawCircleV(Vector2 { center.x + 14 * s, center.y - 4 * s }, 17 * s, Color { 255, 225, 185, 255 });
        DrawLineEx(Vector2 { center.x - 4 * s, center.y + 19 * s }, Vector2 { center.x + 24 * s, center.y + 19 * s }, 7 * s, light);
        DrawLineEx(Vector2 { center.x - 24 * s, center.y + 19 * s }, Vector2 { center.x + 4 * s, center.y + 19 * s }, 7 * s, light);
        break;
    case CardType::DrawFromBottom:
        DrawRectangleRounded(Rectangle { center.x - 30 * s, center.y - 24 * s, 60 * s, 48 * s }, 0.12f, 6, light);
        DrawRectangleRoundedLinesEx(Rectangle { center.x - 30 * s, center.y - 24 * s, 60 * s, 48 * s }, 0.12f, 6, 3 * s, ink);
        DrawLineEx(Vector2 { center.x, center.y + 36 * s }, Vector2 { center.x, center.y + 5 * s }, 5 * s, ink);
        DrawTriangle(Vector2 { center.x - 13 * s, center.y + 11 * s }, Vector2 { center.x + 13 * s, center.y + 11 * s }, Vector2 { center.x, center.y - 6 * s }, ink);
        break;
    case CardType::SeeTheFuture:
        DrawCircleV(center, 29 * s, light);
        DrawCircleV(center, 13 * s, Color { 53, 141, 171, 255 });
        DrawCircleV(center, 5 * s, ink);
        break;
    case CardType::Shuffle:
        DrawLineEx(Vector2 { center.x - 32 * s, center.y - 13 * s }, Vector2 { center.x + 18 * s, center.y - 13 * s }, 5 * s, ink);
        DrawTriangle(Vector2 { center.x + 18 * s, center.y - 25 * s }, Vector2 { center.x + 18 * s, center.y - 1 * s }, Vector2 { center.x + 36 * s, center.y - 13 * s }, ink);
        DrawLineEx(Vector2 { center.x + 32 * s, center.y + 17 * s }, Vector2 { center.x - 18 * s, center.y + 17 * s }, 5 * s, ink);
        DrawTriangle(Vector2 { center.x - 18 * s, center.y + 5 * s }, Vector2 { center.x - 18 * s, center.y + 29 * s }, Vector2 { center.x - 36 * s, center.y + 17 * s }, ink);
        break;
    case CardType::Nope:
        DrawLineEx(Vector2 { center.x - 28 * s, center.y - 28 * s }, Vector2 { center.x + 28 * s, center.y + 28 * s }, 8 * s, Color { 246, 84, 84, 255 });
        DrawLineEx(Vector2 { center.x + 28 * s, center.y - 28 * s }, Vector2 { center.x - 28 * s, center.y + 28 * s }, 8 * s, Color { 246, 84, 84, 255 });
        break;
    case CardType::CatCard:
        DrawCircleV(center, 28 * s, light);
        DrawTriangle(Vector2 { center.x - 22 * s, center.y - 18 * s }, Vector2 { center.x - 8 * s, center.y - 43 * s }, Vector2 { center.x + 1 * s, center.y - 15 * s }, light);
        DrawTriangle(Vector2 { center.x + 22 * s, center.y - 18 * s }, Vector2 { center.x + 8 * s, center.y - 43 * s }, Vector2 { center.x - 1 * s, center.y - 15 * s }, light);
        DrawCircleV(Vector2 { center.x - 10 * s, center.y - 2 * s }, 4 * s, ink);
        DrawCircleV(Vector2 { center.x + 10 * s, center.y - 2 * s }, 4 * s, ink);
        DrawTriangle(Vector2 { center.x - 4 * s, center.y + 7 * s }, Vector2 { center.x + 4 * s, center.y + 7 * s }, Vector2 { center.x, center.y + 13 * s }, Color { 220, 112, 112, 255 });
        break;
    }
}

static bool drawCardWidget(const Card& card, Rectangle rect, bool clickable, const AssetManager& assets)
{
    Vector2 mouse = GetMousePosition();
    bool hover = clickable && pointInRect(mouse, rect);
    Color color = colorForCard(card);
    if (hover) {
        rect.y -= 8;
    }
    DrawRectangleRounded(rect, 0.08f, 12, color);
    DrawRectangleRoundedLinesEx(rect, 0.08f, 12, hover ? 3.0f : 2.0f, Color { 31, 28, 25, 255 });

    if (const Texture2D* texture = assets.textureForCard(card)) {
        Rectangle imageRect { rect.x + 7, rect.y + 7, rect.width - 14, rect.height - 14 };
        drawTextureFit(*texture, imageRect);
        return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }

    DrawRectangleRounded(Rectangle { rect.x + 8, rect.y + 8, rect.width - 16, 28 }, 0.12f, 8, withAlpha(Color { 255, 255, 255, 255 }, 205));

    int titleSize = 16;
    while (titleSize > 9 && MeasureUiText(card.name().c_str(), titleSize) > rect.width - 20) {
        --titleSize;
    }
    DrawUiText(card.name().c_str(), static_cast<int>(rect.x + 12), static_cast<int>(rect.y + 15), titleSize, Color { 31, 28, 25, 255 });
    drawCardIcon(card, Rectangle { rect.x + 14, rect.y + 42, rect.width - 28, rect.height - 80 });
    drawWrappedText(card.description(), rect.x + 12, rect.y + rect.height - 45, rect.width - 24, 11, Color { 31, 28, 25, 255 }, 3);

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void drawDeckPile(Rectangle rect, int count, const std::string& label, Color color)
{
    DrawRectangleRounded(Rectangle { rect.x + 8, rect.y + 8, rect.width, rect.height }, 0.08f, 10, Color { 23, 21, 19, 255 });
    DrawRectangleRounded(rect, 0.08f, 10, color);
    DrawRectangleRoundedLinesEx(rect, 0.08f, 10, 2.0f, Color { 31, 28, 25, 255 });
    int labelWidth = MeasureUiText(label.c_str(), 20);
    DrawUiText(label.c_str(), static_cast<int>(rect.x + (rect.width - labelWidth) / 2), static_cast<int>(rect.y + 20), 20, Color { 31, 28, 25, 255 });
    std::string countText = std::to_string(count);
    int countWidth = MeasureUiText(countText.c_str(), 46);
    DrawUiText(countText.c_str(), static_cast<int>(rect.x + (rect.width - countWidth) / 2), static_cast<int>(rect.y + 58), 46, Color { 31, 28, 25, 255 });
}

static void drawPlayerPanels(GameManager& gm, float screenWidth)
{
    const float gap = 12;
    const int playerCount = std::max(1, static_cast<int>(gm.players.size()));
    const float panelWidth = (screenWidth - 40 - gap * (playerCount - 1)) / playerCount;
    for (int i = 0; i < static_cast<int>(gm.players.size()); ++i) {
        Rectangle rect { 20 + i * (panelWidth + gap), 74, panelWidth, 72 };
        const Player& player = gm.players[i];
        Color fill = i == gm.currentPlayerIndex ? Color { 76, 104, 96, 255 } : Color { 48, 44, 40, 255 };
        if (!player.isAlive) {
            fill = Color { 52, 44, 44, 255 };
        }
        drawPanel(rect, fill);
        DrawUiText(player.name.c_str(), static_cast<int>(rect.x + 14), static_cast<int>(rect.y + 12), 20, RAYWHITE);
        std::string status = player.isAlive ? "Alive" : "Out";
        std::string cards = std::to_string(player.myCard.size()) + " cards";
        DrawUiText(status.c_str(), static_cast<int>(rect.x + 14), static_cast<int>(rect.y + 42), 16, player.isAlive ? Color { 174, 231, 184, 255 } : Color { 247, 128, 128, 255 });
        int cardsWidth = MeasureUiText(cards.c_str(), 16);
        DrawUiText(cards.c_str(), static_cast<int>(rect.x + rect.width - cardsWidth - 14), static_cast<int>(rect.y + 42), 16, Color { 235, 225, 205, 255 });
    }
}

static void drawLogPanel(GameManager& gm, Rectangle rect)
{
    drawPanel(rect, Color { 38, 35, 32, 255 });
    DrawUiText("Game Log", static_cast<int>(rect.x + 16), static_cast<int>(rect.y + 16), 22, RAYWHITE);
    float y = rect.y + 52;
    for (const auto& line : gm.logs()) {
        drawWrappedText(line, rect.x + 16, y, rect.width - 32, 15, Color { 229, 219, 203, 255 }, 2);
        y += 42;
        if (y > rect.y + rect.height - 24) {
            break;
        }
    }
}

static void drawHand(GameManager& gm, Rectangle area, const AssetManager& assets)
{
    drawPanel(area, Color { 41, 38, 34, 255 });
    const Player& player = gm.players[gm.currentPlayerIndex];
    DrawUiText((player.name + "'s Hand").c_str(), static_cast<int>(area.x + 16), static_cast<int>(area.y + 14), 22, RAYWHITE);

    int n = static_cast<int>(player.myCard.size());
    if (n == 0) {
        DrawUiText("No cards.", static_cast<int>(area.x + 16), static_cast<int>(area.y + 58), 18, Color { 210, 200, 185, 255 });
        return;
    }

    int rows = n > 7 ? 2 : 1;
    int perRow = rows == 1 ? n : static_cast<int>(std::ceil(n / 2.0f));
    float gap = 10;
    float rowSlotHeight = (area.height - 74 - gap * (rows - 1)) / rows;
    float maxCardWidth = rows == 1 ? 166.0f : 132.0f;
    float availableCardWidth = (area.width - 32 - gap * (perRow - 1)) / perRow;
    float cardWidth = std::min(maxCardWidth, availableCardWidth);
    float cardHeight = cardWidth * 1.42f;
    if (cardHeight > rowSlotHeight) {
        cardHeight = rowSlotHeight;
        cardWidth = cardHeight / 1.42f;
    }
    cardWidth = std::min(std::max(72.0f, cardWidth), availableCardWidth);
    cardHeight = cardWidth * 1.42f;

    for (int i = 0; i < n; ++i) {
        int row = rows == 1 ? 0 : i / perRow;
        int col = rows == 1 ? i : i % perRow;
        Rectangle rect {
            area.x + 16 + col * (cardWidth + gap),
            area.y + 54 + row * (cardHeight + gap),
            cardWidth,
            cardHeight
        };
        if (drawCardWidget(*player.myCard[i], rect, gm.phase() == GamePhase::Playing, assets)) {
            gm.playCard(i);
            return;
        }
    }
}

static void drawActionArea(GameManager& gm, Rectangle rect)
{
    drawPanel(rect, Color { 41, 38, 34, 255 });
    DrawUiText(gm.banner().c_str(), static_cast<int>(rect.x + 16), static_cast<int>(rect.y + 14), 24, RAYWHITE);

    Rectangle deckRect { rect.x + 26, rect.y + 62, 128, 156 };
    Rectangle discardRect { rect.x + 184, rect.y + 62, 128, 156 };
    drawDeckPile(deckRect, gm.deck_count, "Deck", Color { 244, 202, 82, 255 });
    drawDeckPile(discardRect, gm.discardPile_count, "Discard", Color { 170, 126, 207, 255 });

    if (!gm.discardPile.empty()) {
        DrawUiText(gm.discardPile.back()->name().c_str(), static_cast<int>(discardRect.x + 10), static_cast<int>(discardRect.y + discardRect.height - 28), 13, Color { 31, 28, 25, 255 });
    }

    float bx = rect.x + 345;
    float by = rect.y + 65;
    bool playing = gm.phase() == GamePhase::Playing;
    if (drawButton(Rectangle { bx, by, 170, 42 }, "Draw", playing && gm.deck_count > 0)) {
        gm.drawCurrentPlayer(false);
    }
    if (drawButton(Rectangle { bx, by + 50, 170, 42 }, "2 Same Cats", playing && gm.specialRules[0]->checkCondition(gm.players[gm.currentPlayerIndex]))) {
        gm.requestTwoCatsRule();
    }
    if (drawButton(Rectangle { bx, by + 100, 170, 42 }, "3 Same Cats", playing && gm.specialRules[1]->checkCondition(gm.players[gm.currentPlayerIndex]))) {
        gm.requestThreeCatsRule();
    }
    if (drawButton(Rectangle { bx, by + 150, 170, 42 }, "5 Different Cats", playing && gm.specialRules[2]->checkCondition(gm.players[gm.currentPlayerIndex]) && !gm.discardPile.empty())) {
        gm.requestFiveCatsRule();
    }
}

static void drawModalBase(const std::string& title, const std::string& subtitle)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    DrawRectangle(0, 0, w, h, Color { 0, 0, 0, 165 });
    Rectangle panel { w / 2.0f - 260, h / 2.0f - 170, 520, 340 };
    drawPanel(panel, Color { 48, 44, 40, 255 });
    int titleWidth = MeasureUiText(title.c_str(), 30);
    DrawUiText(title.c_str(), static_cast<int>(panel.x + (panel.width - titleWidth) / 2), static_cast<int>(panel.y + 28), 30, RAYWHITE);
    if (!subtitle.empty()) {
        int subWidth = MeasureUiText(subtitle.c_str(), 18);
        DrawUiText(subtitle.c_str(), static_cast<int>(panel.x + (panel.width - subWidth) / 2), static_cast<int>(panel.y + 70), 18, Color { 221, 210, 195, 255 });
    }
}

static void drawPassDevice(GameManager& gm)
{
    std::string title = "Pass to " + gm.players[gm.currentPlayerIndex].name;
    drawModalBase(title, "Click reveal when this player is ready.");
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    if (drawButton(Rectangle { w / 2.0f - 100, h / 2.0f + 54, 200, 52 }, "Reveal Hand")) {
        gm.revealCurrentHand();
    }
}

static void drawTargetModal(GameManager& gm)
{
    drawModalBase(gm.banner(), "Choose one living opponent.");
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float startY = h / 2.0f - 54;
    for (int i = 0; i < static_cast<int>(gm.players.size()); ++i) {
        bool enabled = i != gm.currentPlayerIndex && gm.players[i].isAlive;
        if (drawButton(Rectangle { w / 2.0f - 140, startY + i * 48, 280, 38 }, gm.players[i].name, enabled)) {
            gm.chooseTarget(i);
        }
    }
}

static std::vector<std::string> cardNameOptions()
{
    return {
        toString(CardType::Defuse),
        toString(CardType::Skip),
        toString(CardType::Favor),
        toString(CardType::DrawFromBottom),
        toString(CardType::SeeTheFuture),
        toString(CardType::Shuffle),
        toString(CardType::Nope),
        "Cat1",
        "Cat2",
        "Cat3",
        "Cat4",
        "Cat5"
    };
}

static void drawChooseCardNameModal(GameManager& gm)
{
    drawModalBase("Name a Card", "If the target has it, you steal it.");
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    auto options = cardNameOptions();
    float buttonW = 150;
    float buttonH = 36;
    float gap = 12;
    float startX = w / 2.0f - (buttonW * 3 + gap * 2) / 2;
    float startY = h / 2.0f - 62;

    for (int i = 0; i < static_cast<int>(options.size()); ++i) {
        int row = i / 3;
        int col = i % 3;
        if (drawButton(Rectangle { startX + col * (buttonW + gap), startY + row * (buttonH + gap), buttonW, buttonH }, options[i])) {
            gm.chooseNamedCard(options[i]);
        }
    }
}

static void drawDiscardChoiceModal(GameManager& gm)
{
    drawModalBase("Pick From Discard", "Five different cats trade for one discard card.");
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float panelX = w / 2.0f - 230;
    float startY = h / 2.0f - 64;
    int shown = 0;
    for (int i = static_cast<int>(gm.discardPile.size()) - 1; i >= 0 && shown < 8; --i, ++shown) {
        if (drawButton(Rectangle { panelX, startY + shown * 38, 460, 32 }, gm.discardPile[i]->name())) {
            gm.chooseDiscardCard(i);
            return;
        }
    }
}

static void drawNopeModal(GameManager& gm)
{
    const int responder = gm.nopeResponder();
    std::string name = responder >= 0 ? gm.players[responder].name : "Target";
    drawModalBase(name + " can Nope", "Use Nope to cancel this action.");
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    if (drawButton(Rectangle { w / 2.0f - 170, h / 2.0f + 38, 150, 48 }, "Use Nope")) {
        gm.respondNope(true);
    }
    if (drawButton(Rectangle { w / 2.0f + 20, h / 2.0f + 38, 150, 48 }, "Pass")) {
        gm.respondNope(false);
    }
}

static void drawFutureModal(GameManager& gm)
{
    drawModalBase("See the Future", "Top of the deck is listed first.");
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float y = h / 2.0f - 58;
    int index = 1;
    for (const auto& card : gm.futureCards()) {
        std::string line = std::to_string(index++) + ". " + card;
        int lineWidth = MeasureUiText(line.c_str(), 24);
        DrawUiText(line.c_str(), w / 2 - lineWidth / 2, static_cast<int>(y), 24, RAYWHITE);
        y += 42;
    }
    if (drawButton(Rectangle { w / 2.0f - 90, h / 2.0f + 96, 180, 44 }, "Close")) {
        gm.closeFutureView();
    }
}

static void drawBoomPositionModal(GameManager& gm)
{
    drawModalBase("Boom Defused", "Choose where to put the Boom back.");
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    if (drawButton(Rectangle { w / 2.0f - 210, h / 2.0f + 42, 130, 46 }, "Top")) {
        gm.placePendingBoom(0);
    }
    if (drawButton(Rectangle { w / 2.0f - 65, h / 2.0f + 42, 130, 46 }, "Middle")) {
        gm.placePendingBoom(1);
    }
    if (drawButton(Rectangle { w / 2.0f + 80, h / 2.0f + 42, 130, 46 }, "Bottom")) {
        gm.placePendingBoom(2);
    }
}

static bool drawGameOverModal(GameManager& gm)
{
    drawModalBase(gm.banner(), "Return to setup to play again.");
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    return drawButton(Rectangle { w / 2.0f - 100, h / 2.0f + 54, 200, 52 }, "Main Menu");
}

} // namespace

CatBombApp::CatBombApp()
{
    assets_.load();
    setUiFont(assets_.font());
}

void CatBombApp::drawBackgroundTint()
{
    const int w = GetScreenWidth();
    const int h = GetScreenHeight();
    if (const Texture2D* background = assets_.background()) {
        drawTextureCover(*background, Rectangle { 0, 0, static_cast<float>(w), static_cast<float>(h) });
    } else {
        ClearBackground(Color { 30, 28, 25, 255 });
    }
    DrawRectangle(0, 0, w, h, Color { 18, 16, 14, 128 });
}

void CatBombApp::drawStartScreen()
{
    drawBackgroundTint();
    const int h = GetScreenHeight();
    const int w = GetScreenWidth();

    DrawUiText("Cat Bomb", 54, 54, 58, RAYWHITE);
    DrawUiText("C++ GUI card game", 58, 118, 24, Color { 255, 238, 207, 255 });
    if (drawHelpButton(static_cast<float>(w))) {
        showCardHelp_ = true;
    }

    Rectangle panel { 54, h - 220.0f, 420, 150 };
    drawPanel(panel, Color { 38, 35, 32, 220 });
    DrawUiText("Ready to play?", static_cast<int>(panel.x + 26), static_cast<int>(panel.y + 24), 28, RAYWHITE);
    DrawUiText("Set players and names before the first draw.", static_cast<int>(panel.x + 26), static_cast<int>(panel.y + 64), 17, Color { 229, 219, 203, 255 });

    if (drawButton(Rectangle { panel.x + 26, panel.y + 96, 180, 42 }, "Start Game")) {
        screen_ = Screen::Setup;
        activeNameIndex_ = 0;
    }
    if (showCardHelp_) {
        drawCardHelpModal();
    }
}

void CatBombApp::handleNameInput()
{
    activeNameIndex_ = std::clamp(activeNameIndex_, 0, playerCount_ - 1);
    std::string& name = playerNames_[activeNameIndex_];

    int codepoint = GetCharPressed();
    while (codepoint > 0) {
        if (codepoint >= 32 && codepoint <= 126 && name.size() < 18) {
            name.push_back(static_cast<char>(codepoint));
        }
        codepoint = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !name.empty()) {
        name.pop_back();
    }
    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_DOWN)) {
        activeNameIndex_ = (activeNameIndex_ + 1) % playerCount_;
    }
    if (IsKeyPressed(KEY_UP)) {
        activeNameIndex_ = (activeNameIndex_ + playerCount_ - 1) % playerCount_;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        startConfiguredGame();
    }
}

void CatBombApp::drawSetupScreen()
{
    drawBackgroundTint();
    handleNameInput();

    const int w = GetScreenWidth();
    const int h = GetScreenHeight();
    if (drawHelpButton(static_cast<float>(w))) {
        showCardHelp_ = true;
    }
    Rectangle panel { w / 2.0f - 340, h / 2.0f - 280, 680, 560 };
    drawPanel(panel, Color { 40, 36, 32, 232 });

    DrawUiText("Game Setup", static_cast<int>(panel.x + 32), static_cast<int>(panel.y + 28), 34, RAYWHITE);
    DrawUiText("Choose player count and edit each player name.", static_cast<int>(panel.x + 34), static_cast<int>(panel.y + 72), 18, Color { 229, 219, 203, 255 });

    DrawUiText("Players", static_cast<int>(panel.x + 34), static_cast<int>(panel.y + 124), 22, RAYWHITE);
    float selectorX = panel.x + 150;
    for (int count = 2; count <= 4; ++count) {
        bool selected = playerCount_ == count;
        Rectangle button { selectorX + (count - 2) * 76.0f, panel.y + 116, 62, 40 };
        if (drawButton(button, std::to_string(count), true)) {
            playerCount_ = count;
            activeNameIndex_ = std::min(activeNameIndex_, playerCount_ - 1);
        }
        if (selected) {
            DrawRectangleRoundedLinesEx(button, 0.12f, 8, 4.0f, Color { 255, 255, 255, 255 });
        }
    }

    float startY = panel.y + 188;
    for (int i = 0; i < playerCount_; ++i) {
        Rectangle input { panel.x + 34, startY + i * 66, panel.width - 68, 48 };
        bool active = activeNameIndex_ == i;
        Vector2 mouse = GetMousePosition();
        if (pointInRect(mouse, input) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            activeNameIndex_ = i;
        }
        DrawRectangleRounded(input, 0.10f, 8, active ? Color { 255, 238, 207, 245 } : Color { 236, 220, 198, 218 });
        DrawRectangleRoundedLinesEx(input, 0.10f, 8, active ? 3.0f : 1.5f, active ? Color { 244, 202, 82, 255 } : Color { 91, 78, 66, 255 });

        std::string label = "P" + std::to_string(i + 1);
        DrawUiText(label.c_str(), static_cast<int>(input.x + 16), static_cast<int>(input.y + 14), 20, Color { 57, 47, 39, 255 });
        DrawUiText(playerNames_[i].c_str(), static_cast<int>(input.x + 64), static_cast<int>(input.y + 14), 20, Color { 31, 28, 25, 255 });
        if (active && (GetTime() - std::floor(GetTime())) < 0.55) {
            int textWidth = MeasureUiText(playerNames_[i].c_str(), 20);
            DrawRectangle(static_cast<int>(input.x + 66 + textWidth), static_cast<int>(input.y + 12), 2, 25, Color { 31, 28, 25, 255 });
        }
    }

    Rectangle backButton { panel.x + 34, panel.y + panel.height - 72, 150, 44 };
    Rectangle startButton { panel.x + panel.width - 234, panel.y + panel.height - 72, 200, 44 };
    if (drawButton(backButton, "Back")) {
        screen_ = Screen::Start;
    }
    if (drawButton(startButton, "Begin Match")) {
        startConfiguredGame();
    }
    if (showCardHelp_) {
        drawCardHelpModal();
    }
}

void CatBombApp::startConfiguredGame()
{
    std::vector<std::string> names;
    names.reserve(playerCount_);
    for (int i = 0; i < playerCount_; ++i) {
        names.push_back(playerNames_[i].empty() ? "Player " + std::to_string(i + 1) : playerNames_[i]);
    }
    GameManager::instance().startGame(names);
    screen_ = Screen::Game;
}

void CatBombApp::drawCardHelpModal()
{
    const int w = GetScreenWidth();
    const int h = GetScreenHeight();
    DrawRectangle(0, 0, w, h, Color { 0, 0, 0, 170 });

    Rectangle panel {
        w / 2.0f - 430,
        h / 2.0f - 315,
        860,
        630
    };
    drawPanel(panel, Color { 42, 38, 34, 246 });
    DrawUiText("Card Guide", static_cast<int>(panel.x + 28), static_cast<int>(panel.y + 24), 34, RAYWHITE);
    DrawUiText("Quick reference for every card and cat combo.", static_cast<int>(panel.x + 30), static_cast<int>(panel.y + 66), 18, Color { 229, 219, 203, 255 });

    struct HelpLine {
        const char* name;
        const char* detail;
    };

    const std::array<HelpLine, 9> cards {
        HelpLine { "Boom", "Draw it and you explode unless you have Defuse." },
        HelpLine { "Defuse", "Used automatically against Boom, then place Boom back." },
        HelpLine { "Skip", "End your turn without drawing a card." },
        HelpLine { "Favor", "Choose a player and steal one random card." },
        HelpLine { "Nope", "Cancel Favor or a cat combo when prompted." },
        HelpLine { "Shuffle", "Shuffle the draw pile." },
        HelpLine { "See the Future", "Peek at the top three cards." },
        HelpLine { "Draw from Bottom", "Draw the bottom card and end your turn." },
        HelpLine { "Cat1 - Cat5", "Normal cat cards. Use them only for combos." }
    };

    const std::array<HelpLine, 3> combos {
        HelpLine { "2 same cats", "Discard two matching cats to steal a random card." },
        HelpLine { "3 same cats", "Discard three matching cats, name a card, steal it if found." },
        HelpLine { "5 different cats", "Discard Cat1-Cat5 to take one card from discard." }
    };

    float leftX = panel.x + 34;
    float rightX = panel.x + 462;
    float y = panel.y + 112;
    DrawUiText("Cards", static_cast<int>(leftX), static_cast<int>(y), 24, RAYWHITE);
    y += 38;
    for (const auto& line : cards) {
        DrawUiText(line.name, static_cast<int>(leftX), static_cast<int>(y), 19, Color { 255, 226, 121, 255 });
        drawWrappedText(line.detail, leftX + 155, y + 1, 245, 16, Color { 234, 225, 211, 255 }, 2);
        y += 44;
    }

    y = panel.y + 112;
    DrawUiText("Combos", static_cast<int>(rightX), static_cast<int>(y), 24, RAYWHITE);
    y += 38;
    for (const auto& line : combos) {
        DrawUiText(line.name, static_cast<int>(rightX), static_cast<int>(y), 19, Color { 255, 226, 121, 255 });
        drawWrappedText(line.detail, rightX, y + 26, 340, 16, Color { 234, 225, 211, 255 }, 3);
        y += 92;
    }

    DrawUiText("Image filenames: cat1.png to cat5.png are five different normal cat cards.", static_cast<int>(rightX), static_cast<int>(panel.y + panel.height - 108), 16, Color { 229, 219, 203, 255 });

    if (drawButton(Rectangle { panel.x + panel.width - 142, panel.y + panel.height - 62, 108, 38 }, "Close")) {
        showCardHelp_ = false;
    }
}

void CatBombApp::drawGameScreen()
{
    drawBackgroundTint();
    GameManager& gm = GameManager::instance();

    const int w = GetScreenWidth();
    const int h = GetScreenHeight();

    DrawRectangle(0, 0, w, 58, Color { 192, 64, 61, 222 });
    DrawUiText("Cat Bomb", 24, 16, 30, RAYWHITE);
    std::string subtitle = std::to_string(gm.players.size()) + "-player local C++ GUI";
    int subtitleWidth = MeasureUiText(subtitle.c_str(), 18);
    if (drawHelpButton(static_cast<float>(w))) {
        showCardHelp_ = true;
    }
    DrawUiText(subtitle.c_str(), w - subtitleWidth - 76, 23, 18, Color { 255, 238, 207, 255 });

    drawPlayerPanels(gm, static_cast<float>(w));

    float rightPanelW = std::min(340.0f, w * 0.26f);
    Rectangle logRect { w - rightPanelW - 20, 164, rightPanelW, h - 184.0f };
    Rectangle actionRect { 20, 164, w - rightPanelW - 60, 250 };
    float handHeight = std::min(350.0f, std::max(282.0f, h * 0.38f));
    Rectangle handRect { 20, h - handHeight - 20.0f, w - rightPanelW - 60, handHeight };

    drawActionArea(gm, actionRect);
    drawHand(gm, handRect, assets_);
    drawLogPanel(gm, logRect);

    switch (gm.phase()) {
    case GamePhase::PassDevice:
        drawPassDevice(gm);
        break;
    case GamePhase::ChooseTarget:
        drawTargetModal(gm);
        break;
    case GamePhase::ChooseCardName:
        drawChooseCardNameModal(gm);
        break;
    case GamePhase::ChooseDiscardCard:
        drawDiscardChoiceModal(gm);
        break;
    case GamePhase::ChooseNopeResponse:
        drawNopeModal(gm);
        break;
    case GamePhase::FutureView:
        drawFutureModal(gm);
        break;
    case GamePhase::ChooseBoomPosition:
        drawBoomPositionModal(gm);
        break;
    case GamePhase::GameOver:
        if (drawGameOverModal(gm)) {
            screen_ = Screen::Start;
        }
        break;
    case GamePhase::Playing:
        break;
    }

    if (showCardHelp_) {
        drawCardHelpModal();
    }
}

void CatBombApp::updateAndDraw()
{
    assets_.updateMusic();
    BeginDrawing();
    ClearBackground(Color { 30, 28, 25, 255 });

    switch (screen_) {
    case Screen::Start:
        drawStartScreen();
        break;
    case Screen::Setup:
        drawSetupScreen();
        break;
    case Screen::Game:
        drawGameScreen();
        break;
    }

    EndDrawing();
}
