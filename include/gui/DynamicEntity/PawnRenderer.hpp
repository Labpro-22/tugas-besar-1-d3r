#pragma once
#include "../include/core/Player.hpp"
#include "raylib.h"

class PawnRenderer {
private:
    Texture2D spriteSheets;
    static PawnRenderer* instance;
    int frameWidth;
    int frameHeight;
    PawnRenderer(const string path);

public:
    ~PawnRenderer();
    static PawnRenderer& getInstance();
    void DrawPawn(int tileIndex, int playerIndex);
};
