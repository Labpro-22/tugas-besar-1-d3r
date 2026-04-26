#pragma once

#include "../RenderConfig.hpp"
#include "../../core/Tile.hpp"
#include "raylib.h"
#include "../include/gui/MathCore/IsoTransformer.hpp"
#include "../include/core/GameManager.hpp"
#include <string>
class TileRenderer {
public:
    static void DrawIsometricTile(Vector2 centerPos, std::pair<Color, Color> tileColors);
    static void FillTileTypes(Vector2 tilePos, int index, Color tileColor);
    static void DrawTextIsometric(const std::string text, Vector2 originOffset,Vector2 tilePos, float rotation);
    static float GetTextRotation(int index);
    // Melakukan parsing tile dan menghasilkan warna utama dan shadenya
    static std::pair<Color, Color> ParseColor(Tile *tile);
    static int getBoardSize() { return GameManager::getInstance().getBoard().getTiles().size(); }
};
