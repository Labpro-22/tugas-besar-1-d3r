#pragma once
#include "../RenderConfig.hpp"
#include "raylib.h"

struct RenderConfig;

class IsoTransformer {
public:
    static Vector2 IndexToGrid(int index);
    static Vector2 GetScreenPosition(int index);
};
