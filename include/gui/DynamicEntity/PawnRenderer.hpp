#include "raylib.h"
#include "../include/core/Player.hpp"

class PawnRenderer : public Player {
private:
    Texture2D spriteSheets;
    int frameWidth;
    int frameHeight;

public:
    PawnRenderer(const string path);
    ~PawnRenderer();
    void DrawPawn(int tileIndex, int playerIndex);
};
