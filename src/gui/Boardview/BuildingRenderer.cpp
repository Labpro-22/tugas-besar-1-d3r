#include "../include/gui/NimonspoliGUI.hpp"
// #include "raymath.h"
#include "../include/gui/MathCore/IsoTransformer.hpp"
#include <iostream>

void BuildingRenderer::setContext(int index)
{
    currentPos = IsoTransformer::GetScreenPosition(index);
    currentIndex = index;
}

void BuildingRenderer::drawHouses(int count)
{
    if (count <= 0) return;
    if (count > 3) count = 3;

    float width = RenderConfig::TILE_WIDTH;
    float height = RenderConfig::TILE_HEIGHT;

    GameManager &game = GameManager::getInstance();
    int BoardSize = game.getBoard().getTiles().size();
    int base = BoardSize / 4;

    // Titik-titik referensi ubin
    Vector2 top = {currentPos.x, currentPos.y - height / 2.0f};
    Vector2 bottom = {currentPos.x, currentPos.y + height / 2.0f};
    Vector2 left = {currentPos.x - width / 2.0f, currentPos.y};
    Vector2 right = {currentPos.x + width / 2.0f, currentPos.y};
    Vector2 mid = currentPos; // Pusat ubin

    std::vector<Vector2> anchors(3);

    auto Vector2Lerp = [](Vector2 s, Vector2 e, float t) {
        return Vector2{s.x + t * (e.x - s.x), s.y + t * (e.y - s.y)};
    };

    if ((currentIndex >= 1 && currentIndex <= base) || (currentIndex > base && currentIndex <= base * 2)) {
        // Sisi Kanan-Bawah (menuju mid dari arah 'right')
        anchors[0] = Vector2Lerp(right, mid, 0.25f);
        anchors[1] = Vector2Lerp(right, mid, 0.50f);
        anchors[2] = Vector2Lerp(right, mid, 0.75f);
    }
    else {
        // Sisi Kanan-Atas (menuju mid dari arah 'left')
        anchors[0] = Vector2Lerp(left, mid, 0.25f);
        anchors[1] = Vector2Lerp(left, mid, 0.50f);
        anchors[2] = Vector2Lerp(left, mid, 0.75f);
    }

    for (int i = 0; i < count; i++) {
        drawIsometricHouse(anchors[i]);
    }
}

void BuildingRenderer::drawIsometricHouse(Vector2 IsoPos)
{
    float sizeHouse = 8.0f;
    GameManager &game = GameManager::getInstance();
    Tile *currentTile = game.getBoard().getTiles().at(currentIndex);
    std::pair<Color, Color> colorsPair = TileRenderer::ParseColor(currentTile);

    // Render Badan Rumah
    // Offset -sizeHouse pada Y agar dasar rumah tepat di IsoPos
    Rectangle rec = {IsoPos.x - sizeHouse / 2, IsoPos.y - sizeHouse, sizeHouse, sizeHouse};
    DrawRectangleRounded(rec, 0.3f, 4, colorsPair.first);
    DrawRectangleRoundedLines(rec, 0.3f, 4, BLACK); // Outline agar tegas

    // Render Atap
    Vector2 v1 = {IsoPos.x - sizeHouse / 2 - 1, IsoPos.y - sizeHouse};
    Vector2 v2 = {IsoPos.x + sizeHouse / 2 + 1, IsoPos.y - sizeHouse};
    Vector2 v3 = {IsoPos.x, IsoPos.y - sizeHouse - 4};
    DrawTriangle(v1, v2, v3, colorsPair.second);
    DrawTriangleLines(v1, v2, v3, BLACK);
}

void BuildingRenderer::drawIsometricStation(Vector2 IsoPos)
{
    float size = 20.0f; // Ukuran dasar
    GameManager &game = GameManager::getInstance();
    Tile *currentTile = game.getBoard().getTiles().at(currentIndex);
    std::pair<Color, Color> colorsPair = TileRenderer::ParseColor(currentTile);
    
    Color detailColor = ColorBrightness(colorsPair.first, -0.3f);

    // Platform Dasar
    // Membuat alas tipis agar gedung tidak melayang
    Rectangle platform = {IsoPos.x - (size + 4) / 2, IsoPos.y - 4, size + 4, 4};
    DrawRectangleRounded(platform, 0.5f, 4, GRAY);
    DrawRectangleRoundedLines(platform, 0.5f, 4,  BLACK);

    // Gedung Utama 
    Rectangle terminal = {IsoPos.x - size / 2, IsoPos.y - size - 2, size * 0.7f, size};
    DrawRectangleRounded(terminal, 0.1f, 4, colorsPair.first);
    DrawRectangleRoundedLines(terminal, 0.1f, 4,  BLACK);
    
    // Menara lagi sure
    float towerSize = size * 0.4f;
    Rectangle tower = {IsoPos.x + 2, IsoPos.y - size - 8, towerSize, size + 6};
    DrawRectangleRounded(tower, 0.2f, 4, colorsPair.second);
    DrawRectangleRoundedLines(tower, 0.2f, 4,  BLACK);

    // Kaca Depan 
    Rectangle window = {IsoPos.x - (size / 2) + 2, IsoPos.y - size + 2, (size * 0.7f) - 4, 6};
    DrawRectangle(window.x, window.y, window.width, window.height, SKYBLUE);

    // Atap Menara
    DrawLineEx({tower.x - 1, tower.y}, {tower.x + towerSize + 1, tower.y}, 2.0f, BLACK);

}

void BuildingRenderer::drawStation()
{
    float width = RenderConfig::TILE_WIDTH;
    float height = RenderConfig::TILE_HEIGHT;

    GameManager &game = GameManager::getInstance();
    int BoardSize = game.getBoard().getTiles().size();
    int base = BoardSize / 4;

    // Titik-titik referensi ubin
    Vector2 top = {currentPos.x, currentPos.y - height / 2.0f};
    Vector2 bottom = {currentPos.x, currentPos.y + height / 2.0f};
    Vector2 left = {currentPos.x - width / 2.0f, currentPos.y};
    Vector2 right = {currentPos.x + width / 2.0f, currentPos.y};
    Vector2 mid = currentPos; // Pusat ubin

    Vector2 leftTopMid = (top + left) / 2;
    Vector2 leftBottomMid = (left + bottom) / 2;
    Vector2 rightTopMid = (right + top) / 2;
    Vector2 rightBottomMid = (right + bottom) / 2;

    if (currentIndex >= 1 && currentIndex <= base) {
        drawIsometricStation((leftTopMid + right) / 2);
    }
    else if (currentIndex > base && currentIndex <= base * 2) {
        drawIsometricStation((leftBottomMid + right) / 2);
    }
    else if (currentIndex > base * 2 && currentIndex <= base * 3) {
        drawIsometricStation((left + rightBottomMid) / 2);
    }
    else {
        drawIsometricStation((left + rightTopMid) / 2);
    }
}

void BuildingRenderer::render(Street *s)
{
    Vector2 originOffset = {0, 40};
    Vector2 originOffset1 = {-15, 20};
    Player *owner = s->getOwner();
    drawHouses(s->getCurrentLevel());
    if (owner != nullptr) {
        TileRenderer::DrawTextIsometric(owner->getUsername(), originOffset, IsoTransformer::GetScreenPosition(s->getIndex()), TileRenderer::GetTextRotation(s->getIndex()));
    }

    TileRenderer::DrawTextIsometric("M" + to_string(s->getLandCost()), originOffset1, IsoTransformer::GetScreenPosition(s->getIndex()), TileRenderer::GetTextRotation(s->getIndex()));
}

void BuildingRenderer::render(Utility *u)
{
    return;
}
void BuildingRenderer::render(Railroad *r)
{
    Vector2 originOffset = {0, 40};
    Vector2 originOffset1 = {-15, 20};
    Vector2 originOffset2 = {-10, 60};
    Player *owner = r->getOwner();
    drawStation();
    if (owner != nullptr) {
        TileRenderer::DrawTextIsometric(owner->getUsername(), originOffset, currentPos, TileRenderer::GetTextRotation(r->getIndex()));
    }

    TileRenderer::DrawTextIsometric("M" + to_string(r->getLandCost()), originOffset1, currentPos, TileRenderer::GetTextRotation(r->getIndex()));
    TileRenderer::DrawTextIsometric(r->getName(), originOffset2, currentPos, TileRenderer::GetTextRotation(r->getIndex()));
}
