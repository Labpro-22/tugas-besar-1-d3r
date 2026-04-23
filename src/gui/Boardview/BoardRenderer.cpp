#include "../include/gui/Boardview/BoardRenderer.hpp"
#include "../include/core/Board.hpp"
#include <iostream>



void renderSingleTile(int index, Board& board) {
    Vector2 pos = IsoTransformer::GetScreenPosition(index);
    Tile *logicTile = board.getTile(index - 1);

    // Kasus tile kosong
    if (logicTile == nullptr) {
        TileRenderer::DrawIsometricTile(pos, LIGHTGRAY);
        return;
    }

    // Logika render utama
    std::pair<Color, Color> c = TileRenderer::ParseColor(logicTile);
    TileRenderer::DrawIsometricTile(pos, c.first);
    TileRenderer::FillTileTypes(pos, index, c.first);
    
    // Ambil rotasi teks (asumsi index untuk rotasi sama dengan index posisi)
    float rotation = TileRenderer::GetTextRotation(index);
    TileRenderer::DrawTextIsometric(logicTile->getCode(), pos, rotation);
}

void BoardRenderer::RenderBoard(Board board)
{
    int boardsize = board.getTiles().size();

    // Render bagian half ke kanan
    for (int i = (boardsize / 2 ) + 1 ; i < boardsize; i++) {
        renderSingleTile(i, board);
    }

    // Render bagian half ke kiri
    for (int i = (boardsize / 2 ); i >= 1; i--) {
        renderSingleTile(i, board);
    }

}