#include "../include/core/DataManager.hpp"
#include "../include/core/GameManager.hpp"

#include <exception>
#include <iostream>

static void runCli(GameManager& gm) {
    gm.getBoard().printBoard();
    gm.runGame();
    if (!gm.getPlayer().empty()) {
        gm.getCommandHandler().commands();
    }
}

static void runGui(GameManager& gm) {
    (void)gm;
    std::cout << "[WARN] GUI route requested, but this build is CLI-only." << std::endl;
}

int main() {
    try {
        const bool useGui = false;

        GameManager& gm = GameManager::getInstance();

        std::string defaultConfigPath = "config/";
        DataManager dm(
            defaultConfigPath + "misc.txt",
            defaultConfigPath + "property.txt",
            defaultConfigPath + "tax.txt",
            defaultConfigPath + "utility.txt",
            defaultConfigPath + "railroad.txt",
            defaultConfigPath + "special.txt",
            defaultConfigPath + "aksi.txt");

        std::cout << "[INFO] Loading game data..." << std::endl;
        dm.load();
        std::cout << "[SUCCESS] Game data loaded successfully!" << std::endl;

        Board b = gm.getBoard();
        gm.initPlayers();

        std::vector<Tile *> tiles = b.getTiles();
        cout << "Board tiles (1-indexed):\n";
        for (size_t i = 1; i < tiles.size(); i++) {
            if (tiles.at(i) != nullptr) {
                cout << i << ": " << tiles.at(i)->getCode() << "\n";
            }
        }

        const int screenWidth = 1280;
        const int screenHeight = 800;
        InitWindow(screenWidth, screenHeight, "Nimonpoli");
        BoardRenderer br;
        // Konfigurasi Kamera (Opsional, agar bisa zoom/pan)
        Camera2D camera = {0};
        camera.target = {(float)screenWidth / 2, (float)screenHeight / 2};
        camera.offset = {(float)screenWidth / 2, (float)screenHeight / 2};
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;

        SetTargetFPS(60);
        PawnRenderer pr("data/assets/players.png");
        GameConsole console({900, 450, 350, 300});
        console.WriteLine("=== NIMONPOLI SYSTEM READY ===");
        console.WriteLine("1. Ketik 'roll' untuk jalan");
        console.WriteLine("2. Ketik 'clear' untuk hapus log");

        // Main Game Loop
        while (!WindowShouldClose()) {

            // Fitur Zoom sederhana dengan Scroll Mouse
            float wheel = GetMouseWheelMove();
            Vector2 mousePos = GetMousePosition();

            if (CheckCollisionPointRec(mousePos, console.getBounds())) {
                if (wheel != 0) {
                    console.HandleScroll(wheel);
                }
            }
            else {
                if (wheel != 0) {
                    float zoomSpeed = 0.05f;
                    camera.zoom += (wheel * zoomSpeed);
                    camera.zoom += wheel * 0.05f;
                    if (camera.zoom < 0.3f) camera.zoom = 0.3f;
                    if (camera.zoom > 3.0f) camera.zoom = 3.0f; // Limit zoom maksimal
                }
            }

            console.Update();

            // Render Logic 
            BeginDrawing();
            ClearBackground(BACKGROUND);

            BeginMode2D(camera);

            // Memanggil fungsi render
            br.RenderBoard(gm.getBoard());

            // console.SetCommandCallback([&](std::string cmd){
            //     if (cmd == "roll")
            //     {
            //         console.WriteLine("Sistem melempar data base dadu");
            //     }else if (cmd == "clear")
            //     {
            //         console.WriteLine("membersihkan data base");
            //     } else{
            //         console.WriteLine("tidak ada function");
            //     }
                
                
            // });

            EndMode2D();

            // Overlay UI Statis (Tidak terpengaruh kamera)
            DrawText("MONOPOLY ISO-ENGINE v0.1", 20, 20, 20, RAYWHITE);
            DrawText("Scroll to Zoom | Right Click to Pan (if implemented)", 20, 50, 10, LIGHTGRAY);
            DrawFPS(screenWidth - 100, 20);
            console.Render();
            EndDrawing();
        }

        CloseWindow();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "[ERROR] Unknown exception occurred!" << std::endl;
        return 1;
    }
}
