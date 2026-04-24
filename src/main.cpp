#include "../include/core/DataManager.hpp"
#include "../include/core/GameManager.hpp"
#include "../include/gui/NimonspoliGUI.hpp"

#include <exception>
#include <iostream>

static void runCli(GameManager &gm)
{
    gm.getBoard().printBoard();
    gm.runGame();
    if (!gm.getPlayer().empty()) {
        gm.getCommandHandler().commands();
    }
}

static void runGui(GameManager &gm)
{
    (void)gm;
    // std::cout << "[WARN] GUI route requested, but this build is CLI-only." << std::endl;

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
    
    // Add players for testing
    Player *player1 = new Player();
    player1->setUsername("Player 1");
    player1->setCurrency(2000000);
    
    Player *player2 = new Player();
    player2->setUsername("Player 2");
    player2->setCurrency(2000000);
    
    std::vector<Player *> testPlayers = {player1, player2};
    gm.setPlayers(testPlayers);
    gm.initPlayers();
    
    // Add house to a Street tile for rendering demo
    
    /* code */
    for (size_t i = 1; i < 10; i++) {
        Tile *testTile = gm.getBoard().getTile(i % 41); // Get tile at index 6
        if (testTile != nullptr) {
            Street *street = dynamic_cast<Street *>(testTile);
            if (street != nullptr) {
                street->setOwner(player1);
                street->setPropertyStatus(OWNED);
                street->setCurrentLevel(3); // Add 2 houses for visualization
            }
        }
    }
    for (size_t i = 11; i < 40; i++) {
        Tile *testTile = gm.getBoard().getTile(i % 41); // Get tile at index 6
        if (testTile != nullptr) {
            Street *street = dynamic_cast<Street *>(testTile);
            if (street != nullptr) {
                street->setOwner(player2);
                street->setPropertyStatus(OWNED);
                street->setCurrentLevel(3); // Add 2 houses for visualization
            }
        }
    }
    cout << "AMANN\n\n\n\n";
    PawnRenderer pawnRenderer("data/assets/players.png");

    //
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
                if (camera.zoom < 0.3f)
                    camera.zoom = 0.3f;
                if (camera.zoom > 3.0f)
                    camera.zoom = 3.0f; // Limit zoom maksimal
            }
        }

        console.Update();

        // Render Logic
        BeginDrawing();
        ClearBackground(BACKGROUND);

        BeginMode2D(camera);

        // Memanggil fungsi render
        br.RenderBoard(gm.getBoard());
        pawnRenderer.DrawPawn(23, 0);
        pawnRenderer.DrawPawn(40, 1);

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
        DrawText("MONOPOLY ISO-ENGINE v0.1", 20, 20, 20, BLACK);
        DrawText("Scroll to Zoom | Right Click to Pan (if implemented)", 20, 50, 10, LIGHTGRAY);
        DrawFPS(screenWidth - 100, 20);
        console.Render();
        EndDrawing();
    }

    CloseWindow();
}

int main()
{
    try {
        const bool useGui = true;

        GameManager &gm = GameManager::getInstance();

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

        if (useGui) {
            runGui(gm);
        }
        else {
            runCli(gm);
        }
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
