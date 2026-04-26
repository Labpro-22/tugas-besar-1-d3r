#include "../include/gui/RenderConfig.hpp"
#include "../include/gui/UIOverlay/UIComponent.hpp"
#include <sstream>
#include <iostream>

TilePopup::TilePopup() : font_(GetFontDefault())
{
    float cx = (GetScreenWidth() - POPUP_W) / 2.0f;
    float cy = (GetScreenHeight() - POPUP_H) / 2.0f;
    cardRect_ = {cx, cy, (float)POPUP_W, (float)POPUP_H};
    btnRect_ = {0,0,0,0};

}

void TilePopup::render()
{
    if (!isVisible()) return;
    drawPopUp();
}

void TilePopup::drawParagraph(int x, int &cursorY, int maxW, int bottomLimit)
{
    auto lines = wrapText(data_.getDescription(), FS_BODY, maxW);
    const int lineH = (int)FS_BODY + 6;
    for (auto &&line : lines) {
        if (cursorY + lineH > bottomLimit) {
            DrawTextEx(font_, "...", {(float)(x + PADDING), (float)cursorY}, FS_BODY, SPACING, Fade(BLACK, 0.30f));
            break;
        }
        DrawTextEx(font_, line.c_str(), {(float)(x + PADDING), (float)cursorY}, FS_BODY, SPACING, Fade(BLACK, 0.20f));
        cursorY += lineH;
    }
}

void TilePopup::drawFields(int x, int &cursorY, int maxW)
{
    const int labelMaxW = (int)(maxW * 0.48f);
    const int valueMaxW = (int)(maxW * 0.48f);
    const int lineH = (int)FS_FIELD + 14;
    const int popupX = x;
    const int popupW = POPUP_W;

    for (const auto &field : data_.getFields()) {
        std::string label = truncateToFit(field.getLabel(), FS_FIELD, labelMaxW);
        std::string value = truncateToFit(field.getValue(), FS_FIELD, valueMaxW);

        DrawTextEx(font_, label.c_str(), {(float)(popupX + PADDING), (float)cursorY}, FS_FIELD, SPACING, Fade(BLACK, 0.3));

        Vector2 vsz = measure(value, FS_FIELD);
        DrawTextEx(font_, value.c_str(),
                   {(float)(popupX + popupW - PADDING - (int)vsz.x),
                    (float)cursorY},
                   FS_FIELD, SPACING, {30, 30, 30, 255});

        cursorY += lineH;
        DrawLineEx({(float)(popupX + PADDING), (float)(cursorY - 4)},
                   {(float)(popupX + popupW - PADDING), (float)(cursorY - 4)},
                   1.0f, Fade(BLACK, 60));
    }
}

void TilePopup::drawPopUp()
{   
    setVisible(true);
    const int x = (int)cardRect_.x;
    const int y = (int)cardRect_.y;
    const int w = (int)cardRect_.width;
    const int maxTxt = w - PADDING * 2;

    // Overlay
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 130});

    // Background
    DrawRectangleRounded(cardRect_, 0.06f, 8, bgColor);
    DrawRectangleRoundedLines(cardRect_, 0.06f, 8, ABU);

    // Header bar
    Rectangle header = {(float)x, (float)y, (float)w, (float)HEADER_H};
    DrawRectangleRounded(header, 0.06f, 8, ColorBrightness(bgColor, -0.40f));

    // Title
    std::string title = truncateToFit(data_.getTitle(), FS_TITLE, maxTxt);
    int titleW = MeasureText(title.c_str(), FS_TITLE);
    DrawText(title.c_str(),
             x + (w - titleW) / 2,
             y + (HEADER_H - FS_TITLE) / 2,
             FS_TITLE, WHITE);

    int cursorY = y + HEADER_H + 10;

    // Subtitle
    std::string subtitle_text = data_.getSubtitle();
    if (!subtitle_text.empty()) {
        int subW = MeasureText(data_.getSubtitle().c_str(), FS_SUBTITLE);
        DrawText(subtitle_text.c_str(),
                 x + (w - subW) / 2,
                 cursorY,
                 FS_SUBTITLE, Fade(BLACK, 0.30));
        cursorY += FS_SUBTITLE + 12;
    }

    // Divider
    DrawLineEx({(float)(x + PADDING), (float)cursorY},
               {(float)(x + w - PADDING), (float)cursorY},
               1.5f, Fade(BLACK, 0.10));
    cursorY += 12;

    // konten bagian bawah
    const int bottomLimit = y + POPUP_H - PADDING - BTN_H - 12;

    if (!data_.getFields().empty())
        drawFields(x, cursorY, maxTxt);

    if (!data_.getFields().empty() && !data_.getDescription().empty()) {
        cursorY += 6;
        DrawLineEx({(float)(x + PADDING), (float)cursorY},
                   {(float)(x + w - PADDING), (float)cursorY},
                   1.0f, {200, 200, 200, 120});
        cursorY += 10;
    }

    if (!data_.getDescription().empty())
        drawParagraph(x, cursorY, maxTxt, bottomLimit);

    // Close buttom
    int btnX = x + (w - BTN_W) / 2;
    int btnY = y + POPUP_H - PADDING - BTN_H;
    btnRect_ = {(float)btnX, (float)btnY, (float)BTN_W, (float)BTN_H};
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnRect_);

    DrawRectangleRounded(btnRect_, 0.35f, 6,
                         hover ? Color{50, 110, 60, 255} : Color{70, 130, 80, 255});

    int closeW = MeasureText("Tutup", FS_FIELD);
    DrawText("Tutup",
             btnX + (BTN_W - closeW) / 2,
             btnY + (BTN_H - FS_FIELD) / 2,
             FS_FIELD, WHITE);
    handleInput();
}

void TilePopup::handleInput()
{
    if (!isVisible()) return;

    if (IsKeyPressed(KEY_ESCAPE)) {
        setVisible(false);
        return;
    }

    if (IsMouseButtonPressed(KEY_ENTER)) {
        Vector2 mouse = GetMousePosition();
        // Klik tombol Tutup
        std::cout << "bisa engga sih? pos " << mouse.x << " " << mouse.y;
        if (CheckCollisionPointRec(mouse, btnRect_)) {
            setVisible(false);
            return;
        }

        // Klik di luar popup
        if (!CheckCollisionPointRec(mouse, cardRect_)) {
            setVisible(false);
        }
    }
}

Vector2 TilePopup::measure(const std::string &text, float fontSize) const
{
    return MeasureTextEx(font_, text.c_str(), fontSize, SPACING);
}

std::string TilePopup::truncateToFit(const std::string &text,
                                     float fontSize, int maxWidth) const
{
    if (measure(text, fontSize).x <= maxWidth) {
        return text;
    }
    std::string result = text;
    while (!result.empty() && measure(result + "...", fontSize).x > maxWidth) {
        result.pop_back();
    }
    return result + "...";
}

std::vector<std::string> TilePopup::wrapText(const std::string &text, float fontSize, int maxWidth) const
{
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string word, current;
    while (stream >> word) {
        std::string candidate = current.empty() ? word : current + " " + word;

        if (measure(candidate, fontSize).x <= maxWidth) {
            current = candidate;
        }
        else {
            if (!current.empty()) lines.push_back(current);
            if (measure(word, fontSize).x > maxWidth)
                word = truncateToFit(word, fontSize, maxWidth);
            current = word;
        }
    }

    if (!current.empty()) lines.push_back(current);
    return lines;
}
