#include "../include/gui/Boardview/BoardRenderer.hpp"
#include "../include/core/Board.hpp"
#include "../include/core/GameManager.hpp"
#include "../include/gui/DynamicEntity/PawnRenderer.hpp"
#include <iostream>

void renderSingleTile(int index, Board &board)
{
    Vector2 pos = IsoTransformer::GetScreenPosition(index);
    Tile *logicTile = board.getTile(index);

    // Kasus tile kosong
    if (logicTile == nullptr) {
        TileRenderer::DrawIsometricTile(pos, std::pair<Color, Color>(LIGHTGRAY, RED));
        return;
    }

    // Logika render utama
    std::pair<Color, Color> colorShade = TileRenderer::ParseColor(logicTile);
    TileRenderer::DrawIsometricTile(pos, std::pair<Color, Color>(BOARD_LINE, BOARD_LINE) );
    TileRenderer::FillTileTypes(pos, index, colorShade.first);

    // Ambil rotasi teks 
    float rotation = TileRenderer::GetTextRotation(index);
    
    BuildingRenderer br;
    br.setContext(index);
    Tile *tile = board.getTile(index);
    Property *prop = dynamic_cast<Property *>(tile);
    if (prop) {
        prop->callViewer(br);
    }else{
        TileRenderer::DrawIsometricTile(pos, colorShade);
    }
    TileRenderer::DrawTextIsometric(logicTile->getCode(), {0, 0}, pos, rotation);
    
}

void BoardRenderer::RenderBoard(Board board)
{
    int boardsize = board.getTiles().size();

    // Render bagian half ke kanan
    for (int i = (boardsize / 2) + 1; i < boardsize; i++) {

        renderSingleTile(i, board);
    }

    // Render bagian half ke kiri
    for (int i = (boardsize / 2); i >= 1; i--) {
        renderSingleTile(i, board);
    }

    std::vector<Player*> players = GameManager::getInstance().getPlayers();
    for (size_t j = 0; j < players.size(); j++) {
        int playerPositionIndex = players.at(j)->getCurrentTile()->getIndex();
        PawnRenderer::getInstance().DrawPawn(playerPositionIndex, j);
    }
}
