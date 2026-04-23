#include "../include/gui/Boardview/TileRenderer.hpp"

#include <cmath>

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

    DrawTriangle(top, left, bottom, BOARD_LINE);
    DrawTriangle(top, bottom, right, BOARD_LINE);

    DrawLineEx(top, right, 1.5f, BOARD_COKLAT);
    DrawLineEx(right, bottom, 1.5f, BOARD_COKLAT);
    DrawLineEx(bottom, left, 1.5f, BOARD_COKLAT);
    DrawLineEx(left, top, 1.5f, BOARD_COKLAT);

    DrawLineEx(left, lowerLeft, 1.5f, BOARD_COKLAT);
    DrawLineEx(bottom, lowerBottom, 1.5f, BOARD_COKLAT);
    DrawLineEx(right, lowerRight, 1.5f, BOARD_COKLAT);
    DrawLineEx(lowerLeft, lowerBottom, 1.5f, BOARD_COKLAT);
    DrawLineEx(lowerBottom, lowerRight, 1.5f, BOARD_COKLAT);
}

void TileRenderer::FillTileTypes(Vector2 tilePos, int index, Color tileColor)
{
    Vector2 top = {tilePos.x, tilePos.y - (RenderConfig::TILE_HEIGHT / 2.0f)};
    Vector2 bottom = {tilePos.x, tilePos.y + (RenderConfig::TILE_HEIGHT / 2.0f)};
    Vector2 left = {tilePos.x - (RenderConfig::TILE_WIDTH / 2.0f), tilePos.y};
    Vector2 right = {tilePos.x + (RenderConfig::TILE_WIDTH / 2.0f), tilePos.y};

    Vector2 midLeftTop = (top + left) / 2.0f;
    Vector2 midRightTop = (top + right) / 2.0f;
    Vector2 midLeftBottom = (left + bottom) / 2.0f;
    Vector2 midRightBottom = (right + bottom) / 2.0f;
    Vector2 mid = (top + bottom) / 2.0f;

    Vector2 lowerLeft = {left.x, left.y + RenderConfig::TILE_HEIGHT / 2.0f};
    Vector2 lowerRight = {right.x, right.y + RenderConfig::TILE_HEIGHT / 2.0f};
    Vector2 lowerBottom = {bottom.x, bottom.y + RenderConfig::TILE_HEIGHT / 2.0f};

    
    int base = TileRenderer::getBoardSize() / 4;
    index = index - 1;
    if (index % base == 0) {
        DrawTriangle(top, left, bottom, tileColor);
        DrawTriangle(top, bottom, right, tileColor);
    }
    else if (index > 0 && index < base) {
        // Kasus untuk render 0 - 11
        DrawTriangle(midRightBottom, right, mid, tileColor);
        DrawTriangle(right, midRightTop, mid, tileColor);

        DrawTriangle(top, midLeftTop, mid, tileColor);
        DrawTriangle(top, mid, midRightTop, tileColor);
    }
    else if (index > base && index < base*2) {
        DrawTriangle(midRightBottom, right, mid, tileColor);
        DrawTriangle(right, midRightTop, mid, tileColor);
        DrawTriangle(mid, midLeftBottom, midRightBottom, tileColor);
        DrawTriangle(midLeftBottom, bottom, midRightBottom, tileColor);

        DrawTriangle(bottom, lowerBottom, lowerRight, tileColor);
        DrawTriangle(bottom, lowerRight, right, tileColor);

        DrawLineEx(bottom, right, 1.5f, BOARD_COKLAT);
    }
    else if (index > base*2 && index <= base*3) {
        DrawTriangle(mid, midLeftBottom, midRightBottom, tileColor);
        DrawTriangle(midLeftBottom, bottom, midRightBottom, tileColor);

        DrawTriangle(left, midLeftBottom, mid, tileColor);
        DrawTriangle(left, mid, midLeftTop, tileColor);

        DrawTriangle(left, lowerLeft, lowerBottom, tileColor);
        DrawTriangle(left, lowerBottom, bottom, tileColor);
        DrawLineEx(left, bottom, 1.5f, BOARD_COKLAT);
    }
    else if (index > base*3 && index <= base*4) {
        DrawTriangle(left, midLeftBottom, mid, tileColor);
        DrawTriangle(left, mid, midLeftTop, tileColor);

        DrawTriangle(midLeftTop, midRightTop, top, tileColor);
        DrawTriangle(midLeftTop, mid, midRightTop, tileColor);
    }
    else if (index == 0 || index == base || index == base*2 || index == base*3) {
        DrawTriangle(top, left, bottom, tileColor);
        DrawTriangle(top, bottom, right, tileColor);
    }
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
    else if (!tile->getCode().compare("BDG") || !tile->getCode().compare("DEN") || !tile->getCode().compare("MTR")) {
        return {HIJAU, HIJAU_SHADE};
    }
    else if (!tile->getCode().compare("JKT") || !tile->getCode().compare("IKN")) {
        return {BIRU_TUA, BIRU_TUA_SHADE};
    }
    else if (!tile->getCode().compare("PLN") || !tile->getCode().compare("PAM")) {
        return {ABU, ABU_SHADE};
    }
    else if (!tile->getCode().compare("GO")) {
        return {GOLD, GOLD};
    }
    else {
        return {BOARD_BASE, BOARD_BASE};
    }
}

void TileRenderer::DrawTextIsometric(const std::string text, Vector2 tilePos, float rotation)
{
    float fontSize = 12.0f;
    
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, 1.0f);
    
    // Origin di tengah teks agar rotasi simetris
    Vector2 origin = { textSize.x / 2.0f, textSize.y / 2.0f - 20 };

    // Gambar teks
    DrawTextPro(
        GetFontDefault(), 
        text.c_str(), 
        tilePos, // Posisi pusat petak
        origin, 
        rotation, 
        fontSize, 
        1.0f, 
        BLACK
    );
}

float TileRenderer::GetTextRotation(int index) {

    int base = TileRenderer::getBoardSize() / 4;
    int adjustedIndex = index % TileRenderer::getBoardSize();
    float sudutDasar = atan2f(RenderConfig::TILE_HEIGHT, RenderConfig::TILE_WIDTH) * (180.0f / PI);

    if (adjustedIndex >= 1 && adjustedIndex <= base + 1) {
        return sudutDasar; // 0 - 10 bagian bawah ke kiri
    } 
    else if (adjustedIndex > base + 1 && adjustedIndex <= base*2 + 1) {
        return sudutDasar + 130; // bagian bawah kiri ke kiri atas
    } 
    else if (adjustedIndex > base*2 + 1 && adjustedIndex <= base* + 1) {
        return -sudutDasar - 130; // bagian kiri atas ke kanan atas
    } 
    else {
        return sudutDasar - 57;  // bagian kanan atas ke kiri bawah
    }
}
