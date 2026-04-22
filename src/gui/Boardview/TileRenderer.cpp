#include "../include/gui/Boardview/TileRenderer.hpp"

void TileRenderer::DrawIsometricTile(Vector2 centerPos, Color tileColor)
{
    Vector2 top = {centerPos.x, centerPos.y - (RenderConfig::TILE_HEIGHT / 2.0f)};
    Vector2 bottom = {centerPos.x, centerPos.y + (RenderConfig::TILE_HEIGHT / 2.0f)};
    Vector2 left = {centerPos.x - (RenderConfig::TILE_WIDTH / 2.0f), centerPos.y};
    Vector2 right = {centerPos.x + (RenderConfig::TILE_WIDTH / 2.0f), centerPos.y};

    Vector2 lowerLeft = {left.x, left.y + RenderConfig::TILE_HEIGHT / 2.0f};
    Vector2 lowerRight = {right.x, right.y + RenderConfig::TILE_HEIGHT / 2.0f};
    Vector2 lowerBottom = {bottom.x, bottom.y + RenderConfig::TILE_HEIGHT / 2.0f};

    DrawTriangle(left, lowerLeft, lowerBottom, BOARD_LINE);
    DrawTriangle(left, lowerBottom, bottom, BOARD_LINE);
    DrawTriangle(bottom, lowerBottom, lowerRight, BOARD_LINE);
    DrawTriangle(bottom, lowerRight, right, BOARD_LINE);

    DrawTriangle(top, left, bottom, tileColor);
    DrawTriangle(top, bottom, right, tileColor);

    // 6. Outline (Hanya pada bagian yang terlihat/tajam)
    DrawLineEx(top, right, 1.5f, BOARD_COKLAT);
    DrawLineEx(right, bottom, 1.5f, BOARD_COKLAT);
    DrawLineEx(bottom, left, 1.5f, BOARD_COKLAT);
    DrawLineEx(left, top, 1.5f, BOARD_COKLAT);

    // Outline tambahan untuk mempertegas sudut bawah
    DrawLineEx(left, lowerLeft, 1.5f, BOARD_COKLAT);
    DrawLineEx(bottom, lowerBottom, 1.5f, BOARD_COKLAT);
    DrawLineEx(right, lowerRight, 1.5f, BOARD_COKLAT);
    DrawLineEx(lowerLeft, lowerBottom, 1.5f, BOARD_COKLAT);
    DrawLineEx(lowerBottom, lowerRight, 1.5f, BOARD_COKLAT);
}

std::pair<Color, Color> TileRenderer::ParseColor(Tile *tile)
{
    // return dari compare adalah 0 jadi biar 1 di-negasi
    if (!tile->getCode().compare("BKS") || !tile->getCode().compare("DPK") || !tile->getCode().compare("BGR")) {
        return {BIRU_MUDA, BIRU_MUDA_SHADE};
    }
    else if (!tile->getCode().compare("TSK") || !tile->getCode().compare("GRT")) {
        return {COKLAT, COKLAT_SHADE};
    }
    else if (!tile->getCode().compare("MGL") || !tile->getCode().compare("SOL") || !tile->getCode().compare("YOG")) {
        return {PINK, PINK_SHADE};
    }
    else if (!tile->getCode().compare("MAL") || !tile->getCode().compare("SMG") || !tile->getCode().compare("SBY")) {
        return {ORANGE, ORANGE_SHADE};
    }
    else if (!tile->getCode().compare("MKS") || !tile->getCode().compare("BLP") || !tile->getCode().compare("MND")) {
        return {MERAH, MERAH_SHADE};
    }
    else if (!tile->getCode().compare("PLB") || !tile->getCode().compare("PKB") || !tile->getCode().compare("MED")) {
        return {KUNING, KUNING_SHADE};
    }
    else if (!tile->getCode().compare("BDG") || !tile->getCode().compare("DEN") || !tile->getCode().compare("MAT")) {
        return {HIJAU, HIJAU_SHADE};
    }
    else if (!tile->getCode().compare("JKT") || !tile->getCode().compare("IKN")) {
        return {BIRU_TUA, BIRU_TUA_SHADE};
    }
    else if (!tile->getCode().compare("PLN") || !tile->getCode().compare("PAM")) {
        return {ABU, ABU_SHADE};
    }
    else if (!tile->getCode().compare("GO")) {
        return {GOLD,GOLD};
    }
    else{
        return {BOARD_BASE, BOARD_BASE};
    }
}
