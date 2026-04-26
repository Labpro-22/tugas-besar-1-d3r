#include "../include/core/Player.hpp"
#include "../include/gui/UIOverlay/UIComponent.hpp"
#include "../include/core/GameManager.hpp"
#include <iterator>

CurrentPlayerStats::CurrentPlayerStats(int playerIndex)
    : font_(GetFontDefault()), playerIndex_(playerIndex)
{
    float px = MARGIN;
    float py = GetScreenHeight() - CARD_H - MARGIN;
    cardRect_ = {px, py, (float)CARD_W, (float)CARD_H};
    setVisible(true);
}

void CurrentPlayerStats::update(const Player *player)
{   
    const auto& players = GameManager::getInstance().getPlayers();
    auto it = std::find(players.begin(), players.end(), player);
    
    if (it != players.end())
        playerIndex_ = (int)std::distance(players.begin(), it);
    else
        playerIndex_ = 0; 
    syncFromPlayer(*player);
}

void CurrentPlayerStats::syncFromPlayer(const Player &player)
{
    data_.setTitle("Turn: " + player.getUsername());
    data_.setSubtitle("");
    data_.clearFields();

    data_.addField(PopUpField("Saldo",
                              "M " + std::to_string(player.getCurrency())));

    data_.addField(PopUpField("Status",
                              statusToString(player.getStatus())));

    if (player.getActiveCardEffect() != NOEFFECT) {
        data_.addField(PopUpField("Kartu",
                                  effectToString(player.getActiveCardEffect(),
                                                 player.getEffectTurns())));
    }

    if (player.getStatus() == JAILED) {
        data_.addField(PopUpField("Sisa penjara",
                                  std::to_string(player.getJailTurn()) + " giliran"));
    }
}

void CurrentPlayerStats::render()
{
    if (!isVisible()) return;
    drawCard();
}

void CurrentPlayerStats::drawCard()
{
    const int x = (int)cardRect_.x;
    const int y = (int)cardRect_.y;
    const int w = (int)cardRect_.width;
    const int maxW = w - PADDING * 2;

    Color playerColor;
    switch (playerIndex_) {
        case 0:
            playerColor = {158,120,173,255};
            break;
        case 1:
            playerColor = {157,186,247,255};
            break;
        case 2: 
            playerColor = {215,215,99,255};
            break;
        case 3:
            playerColor = {199,186,136,255};
            break;
        default:
            break;
    }

    // Background card
    DrawRectangleRounded(cardRect_, 0.12f, 8, playerColor);
    DrawRectangleRoundedLines(cardRect_, 0.12f, 8, ColorBrightness(playerColor, -0.45f));

    // Header — nama player
    Rectangle hdr = {(float)x, (float)y, (float)w, 36.0f};
    DrawRectangleRounded(hdr, 0.12f, 8, ColorBrightness(playerColor, -0.35f));

    std::string name = truncateToFit(data_.getTitle(), FS_NAME, maxW);
    Vector2 nsz = measure(name, FS_NAME);
    DrawTextEx(font_, name.c_str(),
               {(float)(x + (w - (int)nsz.x) / 2),
                (float)(y + (36 - (int)nsz.y) / 2)},
               FS_NAME, SPACING, WHITE);

    // Fields
    int cursorY = y + 36 + PADDING;
    for (const auto &field : data_.getFields()) {
        std::string lbl = truncateToFit(field.getLabel(), FS_FIELD, (int)(maxW * 0.48f));
        std::string val = truncateToFit(field.getValue(), FS_FIELD, (int)(maxW * 0.48f));

        DrawTextEx(font_, lbl.c_str(),
                   {(float)(x + PADDING), (float)cursorY},
                   FS_FIELD, SPACING, {90, 90, 90, 255});

        Vector2 vsz = measure(val, FS_FIELD);
        DrawTextEx(font_, val.c_str(),
                   {(float)(x + w - PADDING - (int)vsz.x), (float)cursorY},
                   FS_FIELD, SPACING, {30, 30, 30, 255});

        cursorY += (int)FS_FIELD + 10;
    }
}

// Static helper
std::string CurrentPlayerStats::statusToString(PLAYER_STATUS s)
{
    switch (s) {
        case ACTIVE:
            return "Aktif";
        case JAILED:
            return "Dipenjara";
        case BANKRUPT:
            return "Bangkrut";
        default:
            return "-";
    }
}

std::string CurrentPlayerStats::effectToString(CARD_EFFECT e, int turns)
{
    std::string base;
    switch (e) {
        case DISCOUNT:
            base = "Diskon";
            break;
        case SHIELD:
            base = "Shield";
            break;
        default:
            return "-";
    }
    return base + " (" + std::to_string(turns) + "t)";
}

Vector2 CurrentPlayerStats::measure(const std::string &text, float fontSize) const
{
    return MeasureTextEx(font_, text.c_str(), fontSize, SPACING);
}

std::string CurrentPlayerStats::truncateToFit(const std::string &text,
                                              float fontSize, int maxWidth) const
{
    if (measure(text, fontSize).x <= maxWidth) return text;
    std::string result = text;
    while (!result.empty() && measure(result + "...", fontSize).x > maxWidth)
        result.pop_back();
    return result + "...";
}