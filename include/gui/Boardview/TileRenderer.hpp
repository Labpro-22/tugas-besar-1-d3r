#pragma once

#include "../RenderConfig.hpp"
#include "../../core/Tile.hpp"
#include "raylib.h"
#include <utility>

class TileRenderer {
public:
    static void DrawIsometricTile(Vector2 centerPos, Color tileColor);
    
    // Melakukan parsing tile dan menghasilkan warna utama dan shadenya
    static std::pair<Color, Color> ParseColor(Tile *tile);
};
