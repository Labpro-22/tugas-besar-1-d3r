#include "../include/gui/MathCore/IsoTransformer.hpp"

Vector2 IsoTransformer::IndexToGrid(int index)
{
    // Papan Monopoli adalah grid 11x11 (0 sampai 10)
    // Asumsi Index 0 (GO) ada di pojok bawah (grid x:10, y:10)
    // Bagian dari kanan bawah ke kiri bawah
    if (index >= 0 && index <= 10) return {10.0f - index, 10.0f};
    // Bagian bawah kiri ke kiri atas
    else if (index > 10 && index <= 20) return {0.0f, 10.0f - (index - 10)};
    // Bagian kiri atas ke kanan atas
    else if (index > 20 && index <= 30) return {(float)(index - 20), 0.0f};
    // bagian kanan atas ke kanan bawah
    else if (index > 30 && index <= 40) return {10.0f, (float)(index - 30)};
    else return {0.0f, 0.0f}; // TODO buat exception
}

Vector2 IsoTransformer::GetScreenPosition(int index)
{
    Vector2 grid = IsoTransformer::IndexToGrid(index);

    // Membuat isometrik
    float screenX = (grid.x - grid.y) * (RenderConfig::TILE_WIDTH / 2.0f);
    float screenY = (grid.x + grid.y) * (RenderConfig::TILE_HEIGHT / 2.0f);

    // Tambahkan offset agar tidak terpotong di pojok kiri atas monitor
    return {screenX + RenderConfig::OFFSET_X, screenY + RenderConfig::OFFSET_Y};
}