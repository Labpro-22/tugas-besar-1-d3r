#include "raylib.h"
#include "../include/core/Player.hpp"

class PawnRenderer {
private:
    Texture2D spriteSheets;
    int frameWidth;
    int frameHeight;

public:
    PawnRenderer(const string path);
    ~PawnRenderer();
    void DrawPawn(int tileIndex, int playerIndex);
};
