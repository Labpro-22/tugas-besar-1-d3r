#pragma once

#include "../RenderConfig.hpp"
#include "../../core/Tile.hpp"
#include "raylib.h"
#include "../include/gui/MathCore/IsoTransformer.hpp"

class TileRenderer {
public:
    static void DrawIsometricTile(Vector2 centerPos, Color tileColor);
    static void FillTileTypes(Vector2 tilePos, int index, Color tileColor);
    
    // Melakukan parsing tile dan menghasilkan warna utama dan shadenya
    static std::pair<Color, Color> ParseColor(Tile *tile);
};
