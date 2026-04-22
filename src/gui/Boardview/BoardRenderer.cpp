#include "../include/gui/Boardview/BoardRenderer.hpp"
#include "../include/core/Board.hpp"
void BoardRenderer::RenderBoard(Board board)
{
    // Cetak dari Bagian board atas ke bagian titik n,n dari kiri
    int boardsize = board.getTiles().size();
    for (int i = boardsize/2 ; i >= 0; i--) {       
        Vector2 pos = IsoTransformer::GetScreenPosition(i);

        Tile* logicTile = board.getTile(i);
        if (logicTile == nullptr) {
            TileRenderer::DrawIsometricTile(pos, LIGHTGRAY);
            continue;
        }
        
        std::pair<Color, Color>  c = TileRenderer::ParseColor(logicTile);
        TileRenderer::DrawIsometricTile(pos, c.first);
    }


    // Cetak dari bagian ke bagian titik n + 1,n - 1 dari kanan 
    for (int i = boardsize/2 + 1; i <  boardsize; i++) {       
        Vector2 pos = IsoTransformer::GetScreenPosition(i);

        Tile* logicTile = board.getTile(i);
        if (logicTile == nullptr) {
            TileRenderer::DrawIsometricTile(pos, LIGHTGRAY);
            continue;
        }
        
        std::pair<Color, Color>  c = TileRenderer::ParseColor(logicTile);
        TileRenderer::DrawIsometricTile(pos, c.first);
    }

    // Update TILE bagian awal agar tidak tertumpuk
    Vector2 pos = IsoTransformer::GetScreenPosition(0);

        Tile* logicTile = board.getTile(0);
        if (logicTile == nullptr) {
            TileRenderer::DrawIsometricTile(pos, LIGHTGRAY);
        }
        
        std::pair<Color, Color>  c = TileRenderer::ParseColor(logicTile);
        TileRenderer::DrawIsometricTile(pos, c.first);


}