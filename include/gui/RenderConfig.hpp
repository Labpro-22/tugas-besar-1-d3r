#pragma once

#include <utility>

#include "raylib.h"

#define COKLAT (Color){139, 94, 60, 255}
#define COKLAT_SHADE (Color){101, 63, 35, 255}
#define BIRU_MUDA (Color){126, 200, 227, 255}
#define BIRU_MUDA_SHADE (Color){78, 155, 185, 255}
#define PINK (Color){244, 167, 185, 255}
#define PINK_SHADE (Color){204, 112, 138, 255}
#define ORANGE (Color){242, 145, 74, 255}
#define ORANGE_SHADE (Color){193, 99, 35, 255}
#define MERAH (Color){224, 85, 85, 255}
#define MERAH_SHADE (Color){168, 45, 45, 255}
#define KUNING (Color){245, 208, 96, 255}
#define KUNING_SHADE (Color){195, 157, 38, 255}
#define HIJAU (Color){114, 196, 114, 255}
#define HIJAU_SHADE (Color){65, 143, 65, 255}
#define BIRU_TUA (Color){46, 95, 158, 255}
#define BIRU_TUA_SHADE (Color){22, 55, 105, 255}
#define BOARD_BASE (Color){235, 225, 210, 255}
#define BOARD_LINE (Color){200, 184, 154, 255}
#define BACKGROUND (Color){235, 225, 210, 255}
#define ABU (Color){ 150, 145, 138, 255 }
#define ABU_SHADE (Color){ 90, 86, 80, 255 }
#define BOARD_COKLAT (Color){ 193, 154, 107, 255 }

struct RenderConfig {
    // Ukuran satu petak dasar (Rasio Isometrik biasanya 2:1)
    static constexpr float TILE_WIDTH = 120.0f;
    static constexpr float TILE_HEIGHT = 60.0f;

    // Offset agar papan berada di tengah layar (Sesuaikan dengan resolusi window
    // Anda)
    static constexpr float OFFSET_X = 640.0f;
    static constexpr float OFFSET_Y = 150.0f;
};
