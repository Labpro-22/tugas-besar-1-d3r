#include "../include/gui/MathCore/IsoTransformer.hpp"
#include "../include/core/GameManager.hpp"

Vector2 IsoTransformer::IndexToGrid(int index)
{
    GameManager& game = GameManager::getInstance();
    int BoardSize = game.getBoard().getTiles().size();
    // Board 1-indexed: index 1-40 (GO=1, PEN=11, PBM=39)
    // Grid 11x11 (0 sampai 10)
    if (index < 1 || index > BoardSize) return {0.0f, 0.0f};
    
    index = index - 1; // Konversi ke 0-based (0-39)
    
    int base = BoardSize/4;
    // Bagian dari kanan bawah ke kiri bawah (0-10)
    if (index >= 0 && index <= base) return {(float)base - index, (float)base};
    // Bagian bawah kiri ke kiri atas (11-20)
    else if (index > base && index <= base*2) return {0.0f, (float)base - (index - (float)base)};
    // Bagian kiri atas ke kanan atas (21-30)
    else if (index > base*2 && index <= base*3) return {(float)(index - base*2), 0.0f};
    // Bagian kanan atas ke kanan bawah (31-39)
    else if (index > base*3 && index < base*4) return {(float)base, (float)(index - base*3)};
    else return {0.0f, 0.0f};
}

Vector2 IsoTransformer::GetScreenPosition(int index)
{
    Vector2 grid = IsoTransformer::IndexToGrid(index);

    // Membuat isometrik
    float screenX = (grid.x - grid.y) * (RenderConfig::TILE_WIDTH / 2.0f);
    float screenY = (grid.x + grid.y) * (RenderConfig::TILE_HEIGHT / 2.0f);

    return {screenX + RenderConfig::OFFSET_X, screenY + RenderConfig::OFFSET_Y};
}
