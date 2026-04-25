#include "gui/UIOverlay/GameConsole.hpp"
#include <algorithm>
#include <cmath>

// Forward Declarations
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);

// Helper Functions for Dynamic Text Rendering

// Draw text using font inside rectangle limits
static void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}

// Draw text using font inside rectangle limits with support for text selection
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text);
    float textOffsetY = 0;
    float textOffsetX = 0.0f;
    float scaleFactor = fontSize / (float)font.baseSize;

    enum { MEASURE_STATE = 0,
           DRAW_STATE = 1 };
    int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;
    int endLine = -1;
    int lastk = -1;

    for (int i = 0, k = 0; i < length; i++, k++) {
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        if (codepoint == 0x3f) codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n') {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;
            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        if (state == MEASURE_STATE) {
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width) {
                endLine = (endLine < 1) ? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length) {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n') state = !state;

            if (state == DRAW_STATE) {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else {
            if (codepoint == '\n') {
                if (!wordWrap) {
                    textOffsetY += (font.baseSize + (float)font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }
            }
            else {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width)) {
                    textOffsetY += (font.baseSize + (float)font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }

                if ((textOffsetY + font.baseSize * scaleFactor) > rec.height) break;

                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength))) {
                    DrawRectangleRec((Rectangle){rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize * scaleFactor}, selectBackTint);
                    isGlyphSelected = true;
                }

                if ((codepoint != ' ') && (codepoint != '\t')) {
                    DrawTextCodepoint(font, codepoint, (Vector2){rec.x + textOffsetX, rec.y + textOffsetY}, fontSize, isGlyphSelected ? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine)) {
                textOffsetY += (font.baseSize + (float)font.baseSize / 2) * scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;
    }
}

GameConsole::GameConsole(Rectangle area, int fSize)
    : bounds(area), fontSize(fSize), maxHistory(100)
{
    resizer = {bounds.x + bounds.width - 17, bounds.y + bounds.height - 17, 14, 14};
    titleBar = {bounds.x, bounds.y, bounds.width, titleBarHeight};
}

std::vector<std::string> GameConsole::WrapText(const std::string& text, float maxWidth)
{
    std::vector<std::string> lines;
    size_t start = 0;

    while (start <= text.size()) {
        size_t newlinePos = text.find('\n', start);
        std::string segment;

        if (newlinePos == std::string::npos) {
            segment = text.substr(start);
            start = text.size() + 1;
        } else {
            segment = text.substr(start, newlinePos - start);
            start = newlinePos + 1;
        }

        if (segment.empty()) {
            lines.push_back("");
            continue;
        }

        std::string currentLine;
        for (char c : segment) {
            std::string nextLine = currentLine + c;
            if (!currentLine.empty() && MeasureText(nextLine.c_str(), fontSize) > maxWidth) {
                lines.push_back(currentLine);
                currentLine.clear();
            }
            currentLine += c;
        }

        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }
    }

    return lines.empty() ? std::vector<std::string>({""}) : lines;
}

void GameConsole::WriteLine(std::string text)
{
    history.push_back(text);
    if (history.size() > maxHistory) history.erase(history.begin());

    // Wrap text and add to display lines
    float maxLineWidth = bounds.width - 20;  // Padding on both sides
    std::vector<std::string> wrappedLines = WrapText(text, maxLineWidth);
    for (const auto& line : wrappedLines) {
        displayLines.push_back(line);
        if (displayLines.size() > maxHistory * 3) {  // Prevent unlimited growth
            displayLines.erase(displayLines.begin());
        }
    }

    AutoScrollToBottom();
}

std::string GameConsole::ReadLineBlocking(const std::string& prompt)
{
    inputMode = InputMode::Prompt;
    promptLabel = prompt;
    promptBuffer.clear();
    blockingInputReady = false;
    blockingInputResult.clear();

    while (!blockingInputReady && !WindowShouldClose()) {
        Update();

        BeginDrawing();
        ClearBackground(BLACK);
        Render();
        EndDrawing();
    }

    inputMode = InputMode::Command;
    promptLabel.clear();

    if (!blockingInputReady) {
        return "";
    }

    std::string result = blockingInputResult;
    blockingInputReady = false;
    blockingInputResult.clear();
    return result;
}


void GameConsole::AutoScrollToBottom()
{
    float historyHeight = bounds.height - 75;
    int lineSpacing = fontSize + 4;
    int visibleLines = (int)(historyHeight / lineSpacing);

    if ((int)displayLines.size() > visibleLines) {
        scrollOffset = displayLines.size() - visibleLines;
    }
    else {
        scrollOffset = 0;
    }
}

void GameConsole::Update()
{
    framesCounter++;

    Vector2 mouse = GetMousePosition();

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        scrollOffset -= (int)wheel;
        if (scrollOffset < 0) scrollOffset = 0;

        float historyHeight = bounds.height - 75;
        int lineSpacing = fontSize + 4;
        int visibleLines = (int)(historyHeight / lineSpacing);
        int maxScroll = (int)displayLines.size() - visibleLines;

        if (maxScroll < 0) maxScroll = 0;
        if (scrollOffset > maxScroll) scrollOffset = maxScroll;
    }

    if (isResizing) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isResizing = false;
        } else {
            float width = bounds.width + (mouse.x - lastMouse.x);
            bounds.width = (width > minWidth) ? ((width < maxWidth) ? width : maxWidth) : minWidth;

            float height = bounds.height + (mouse.y - lastMouse.y);
            bounds.height = (height > minHeight) ? ((height < maxHeight) ? height : maxHeight) : minHeight;
        }
    } else if (isMoving) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isMoving = false;
        } else {
            bounds.x += (mouse.x - lastMouse.x);
            bounds.y += (mouse.y - lastMouse.y);
        }
    } else {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, resizer)) {
            isResizing = true;
        } else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, titleBar)) {
            isMoving = true;
        }
    }

    resizer.x = bounds.x + bounds.width - 17;
    resizer.y = bounds.y + bounds.height - 17;
    titleBar.x = bounds.x;
    titleBar.y = bounds.y;
    titleBar.width = bounds.width;
    lastMouse = mouse;

    std::string& buffer = (inputMode == InputMode::Prompt) ? promptBuffer : commandBuffer;

    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125)) {
            buffer += (char)key;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !buffer.empty()) {
        buffer.pop_back();
    }

    if (IsKeyPressed(KEY_ENTER) && !buffer.empty()) {
        std::string submitted = buffer;
        buffer.clear();

        WriteLine("> " + submitted);

        if (inputMode == InputMode::Prompt) {
            blockingInputResult = submitted;
            blockingInputReady = true;
        } else if (commandCallback != nullptr) {
            commandCallback(submitted);
        } else {
            ProcessCommand(submitted);
        }
    }
}


void GameConsole::ProcessCommand(std::string cmd)
{
    if (cmd == "1") WriteLine("Sistem: Memulai permainan...");
    else if (cmd == "0") WriteLine("Sistem: Keluar dari game...");
    else WriteLine("Sistem: Perintah tidak dikenal.");
}

void GameConsole::Render()
{
    DrawRectangleRec(bounds, ColorAlpha(BLACK, 0.8f));
    DrawRectangleLinesEx(bounds, 2, DARKGRAY);

    Color titleBarColor = isMoving ? Fade(BLUE, 0.7f) : Fade(DARKBLUE, 0.6f);
    DrawRectangleRec(titleBar, titleBarColor);
    DrawRectangleLinesEx(titleBar, 1, SKYBLUE);
    DrawText("NIMONPOLI CONSOLE - Drag to Move", bounds.x + 5, bounds.y + 5, 12, RAYWHITE);

    Rectangle historyArea = {
        bounds.x + 10,
        bounds.y + 35,
        bounds.width - 20,
        bounds.height - 75
    };

    float historyHeight = historyArea.height;
    int lineSpacing = fontSize + 4;
    int visibleLines = (int)(historyHeight / lineSpacing);

    int startIdx = scrollOffset;
    int endIdx = std::min(startIdx + visibleLines, (int)displayLines.size());

    float textY = historyArea.y;
    BeginScissorMode((int)historyArea.x, (int)historyArea.y, (int)historyArea.width, (int)historyArea.height);
    for (int i = startIdx; i < endIdx; i++) {
        if (i < (int)displayLines.size()) {
            DrawText(displayLines[i].c_str(), (int)historyArea.x, (int)textY, fontSize, RAYWHITE);
            textY += lineSpacing;
        }
    }
    EndScissorMode();

    float inputPosY = bounds.y + bounds.height - 30;
    DrawRectangle(bounds.x, inputPosY, bounds.width, 30, ColorAlpha(DARKGRAY, 0.9f));

    const bool isPrompt = (inputMode == InputMode::Prompt);
    const std::string& buffer = isPrompt ? promptBuffer : commandBuffer;

    if (isPrompt && !promptLabel.empty()) {
        DrawText(promptLabel.c_str(), bounds.x + 10, inputPosY - 16, 10, LIGHTGRAY);
    }

    std::string inputShow = (isPrompt ? "PROMPT: " : "CMD: ") + buffer;
    DrawText(inputShow.c_str(), bounds.x + 10, inputPosY + 7, fontSize, YELLOW);

    if (((framesCounter / 20) % 2) == 0) {
        int txtWidth = MeasureText(inputShow.c_str(), fontSize);
        DrawText("_", bounds.x + 10 + txtWidth, inputPosY + 7, fontSize, YELLOW);
    }

    Color resizerColor = isResizing ? Fade(YELLOW, 0.8f) : Fade(DARKGRAY, 0.6f);
    DrawRectangleRec(resizer, resizerColor);
    DrawRectangleLinesEx(resizer, 1, YELLOW);
}

GameConsole::~GameConsole() {}
