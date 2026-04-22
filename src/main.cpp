#include "../include/core/DataManager.hpp"
#include "../include/core/GameManager.hpp"

#include "../include/gui/NimonspoliGUI.hpp"

#include <exception>
#include <iostream>

int main()
{
    try {
        // Get GameManager singleton instance
        GameManager &gm = GameManager::getInstance();

        // Set config paths
        std::string defaultConfigPath = "config/";
        DataManager dm(
            defaultConfigPath + "misc.txt",
            defaultConfigPath + "property.txt",
            defaultConfigPath + "tax.txt",
            defaultConfigPath + "utility.txt",
            defaultConfigPath + "railroad.txt",
            defaultConfigPath + "special.txt");

        // Load all game data (tiles, players, config) to board via GameManager
        std::cout << "[INFO] Loading game data..." << std::endl;
        dm.load(gm);
        std::cout << "[SUCCESS] Game data loaded successfully!" << std::endl;

        Board b = gm.getBoard();
        for (Tile *t : b.getTiles()) {
            if (t != nullptr) {
                cout << t->getCode() << "\n";
            }
        }

        const int screenWidth = 1280;
        const int screenHeight = 800;
        InitWindow(screenWidth, screenHeight, "Nimonpoli");
        BoardRenderer br;
        // 3. Konfigurasi Kamera (Opsional, agar bisa zoom/pan)
        Camera2D camera = {0};
        camera.target = {(float)screenWidth / 2, (float)screenHeight / 2};
        camera.offset = {(float)screenWidth / 2, (float)screenHeight / 2};
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;

        SetTargetFPS(60);

        // Main Game Loop
        while (!WindowShouldClose()) {


            // Fitur Zoom sederhana dengan Scroll Mouse
            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                camera.zoom += wheel * 0.05f;
                if (camera.zoom < 0.1f) camera.zoom = 0.1f;
            }

            // ---- Render Logic ----
            BeginDrawing();
            ClearBackground(BACKGROUND); 

            BeginMode2D(camera);

            // Memanggil fungsi render yang kita buat sebelumnya
            // Fungsi ini akan menggambar 40 petak berdasarkan index
            br.RenderBoard(gm.getBoard());

            // Tambahan: Gambar titik pusat (pusat koordinat board)
            DrawCircle(RenderConfig::OFFSET_X, RenderConfig::OFFSET_Y, 5, RED);

            EndMode2D();

            // Overlay UI Statis (Tidak terpengaruh kamera)
            DrawText("MONOPOLY ISO-ENGINE v0.1", 20, 20, 20, RAYWHITE);
            DrawText("Scroll to Zoom | Right Click to Pan (if implemented)", 20, 50, 10, LIGHTGRAY);
            DrawFPS(screenWidth - 100, 20);

            EndDrawing();
        }

        CloseWindow();

        return 0;
    }
    catch (const std::exception &e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "[ERROR] Unknown exception occurred!" << std::endl;
        return 1;
    }
}
