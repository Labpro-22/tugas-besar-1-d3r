#pragma once
#include "../RenderConfig.hpp"
#include "raylib.h"

struct RenderConfig;

class IsoTransformer {
public:
    static Vector2 IndexToGrid(int index);
    static Vector2 GetScreenPosition(int index);
};

inline Vector2 operator+(Vector2 v1, Vector2 v2)
{
    return Vector2{v1.x + v2.x, v1.y + v2.y};
}
inline Vector2 operator-(Vector2 v1, Vector2 v2)
{
    return Vector2{v1.x - v2.x, v1.y - v2.y};
}
inline Vector2 operator*(Vector2 v1, float scalar)
{
    return Vector2{v1.x * scalar, v1.y * scalar};
}
inline Vector2 operator/(Vector2 v1, float scalar)
{
    return Vector2 { v1.x / scalar, v1.y / scalar };
}

