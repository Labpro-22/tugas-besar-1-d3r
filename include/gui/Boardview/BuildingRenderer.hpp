#pragma once

#include <vector>

class Tile;
struct RenderConfig;

class BuildingRenderer {
private:
    RenderConfig* renderConfig;
    
public:
    BuildingRenderer(RenderConfig* config);
    
    void render();
    void renderHouse(const Tile* tile, int houseCount);
    void renderHotel(const Tile* tile);
    void renderMortgage(const Tile* tile);
    
    void calculateBuildingPosition(const Tile* tile, int& x, int& y);
};
