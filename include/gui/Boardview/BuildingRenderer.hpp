#pragma once

#include <vector>
#include "../../core/Tile.hpp"
#include "../RenderConfig.hpp"

class BuildingRenderer {
public:
    BuildingRenderer(RenderConfig* config);
    
    void render();
    void renderHouse(const Tile* tile, int houseCount);
    void renderHotel(const Tile* tile);
    void renderMortgage(const Tile* tile);
    
    void calculateBuildingPosition(const Tile* tile, int& x, int& y);
};
