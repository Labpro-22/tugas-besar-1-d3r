#include "../include/core/Tile.hpp"

class BuildingRenderer : public PropertyRenderCall {
private:
    Vector2 currentPos;
    int currentIndex;
    void drawHouses(int count);
    void drawStation();
    void drawIsometricHouse(Vector2 IsoPos);
    void drawIsometricStation(Vector2 isoPos);
    void drawIsometricHotel(Vector2 IsoPos);
    

public:
    BuildingRenderer():currentPos({0,0}),currentIndex(0){};
    void setContext(int index);
    void render(Street *s) override;
    void render(Railroad *r) override;
    void render(Utility *u) override;
};
