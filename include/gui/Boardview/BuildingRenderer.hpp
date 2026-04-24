#include "../include/core/Tile.hpp"

class BuildingRenderer : public PropertyRenderCall {
private:
    Vector2 currentPos;
    int currentIndex;
    void printHeader(std::string owner, PROPERTY_STATUS propertyStatus, int landCost);
    void drawHouses(int count);
    void drawStation();
    void drawIsometricHouse(Vector2 IsoPos);
    void drawIsometricStation(Vector2 isoPos);
    

public:
    BuildingRenderer():currentPos({0,0}),currentIndex(0){};
    void setContext(int index);
    void render(Street *s) override;
    void render(Railroad *r) override;
    void render(Utility *u) override;
};
