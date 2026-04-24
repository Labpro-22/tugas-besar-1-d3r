#include "../include/core/DataManager.hpp"
#include "../include/core/GameManager.hpp"
#include "../include/gui/NimonspoliGUI.hpp"

#include <exception>
#include <iostream>

static void runCli(GameManager &gm)
{
    gm.setUseGuiStream(false);
    gm.setOutputCallback(nullptr);
    gm.setInputCallback(nullptr);
    gm.getBoard().printBoard();
    gm.runGame();
    if (!gm.getPlayer().empty()) {
        gm.getCommandHandler().commands();
    }
}

static void runGui(GameManager &gm)
{
    const int screenWidth = 1280;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Nimonpoli");
    BoardRenderer br;
    Camera2D camera = {0};
    camera.target = {(float)screenWidth / 2, (float)screenHeight / 2};
    camera.offset = {(float)screenWidth / 2, (float)screenHeight / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    SetTargetFPS(60);
    PawnRenderer pr("data/assets/players.png");
    GameConsole console({900, 450, 350, 300});

    gm.setUseGuiStream(true);
    gm.setOutputCallback([&console](const std::string &text)
                         { console.WriteLine(text); });
    gm.setInputCallback([&console](const std::string &prompt)
                        { return console.ReadLineBlocking(prompt); });

    gm.writeLine("=== NIMONPOLI SYSTEM READY ===");
    gm.writeLine("Ketik command seperti CLI di console.");
    gm.writeLine("Contoh: CETAK_PAPAN, LEMPAR_DADU, STATUS");

    gm.getBoard().printBoard();
    gm.runGame();

    console.SetCommandCallback([&gm, &console](std::string cmd)
                               {
        if (!gm.getCommandHandler().execute(cmd))
        {
            console.WriteLine("Command loop selesai.");
        } });

    while (!WindowShouldClose())
    {
        float wheel = GetMouseWheelMove();
        Vector2 mousePos = GetMousePosition();

        if (CheckCollisionPointRec(mousePos, console.getBounds())) {
            if (wheel != 0) {
                console.HandleScroll(wheel);
            }
        }
        else if (wheel != 0)
        {
            float zoomSpeed = 0.05f;
            camera.zoom += wheel * zoomSpeed;
            if (camera.zoom < 0.3f)
            {
                camera.zoom = 0.3f;
            }
            if (camera.zoom > 3.0f)
            {
                camera.zoom = 3.0f;
            }
        }

        console.Update();

        BeginDrawing();
        ClearBackground(BACKGROUND);
        BeginMode2D(camera);
        br.RenderBoard(gm.getBoard());
        EndMode2D();
        DrawText("MONOPOLY ISO-ENGINE v0.1", 20, 20, 20, RAYWHITE);
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
        gm.setUseGuiStream(useGui);

        std::string defaultConfigPath = "config/";
        DataManager dm(
            defaultConfigPath + "misc.txt",
            defaultConfigPath + "property.txt",
            defaultConfigPath + "tax.txt",
            defaultConfigPath + "utility.txt",
            defaultConfigPath + "railroad.txt",
            defaultConfigPath + "special.txt",
            defaultConfigPath + "aksi.txt");

        gm.writeLine("[INFO] Loading game data...");
        dm.load();
        gm.writeLine("[SUCCESS] Game data loaded successfully!");

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
