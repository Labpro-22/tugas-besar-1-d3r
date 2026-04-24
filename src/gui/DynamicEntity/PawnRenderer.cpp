#include "../include/gui/DynamicEntity/PawnRenderer.hpp"
#include "../include/core/GameManager.hpp"
#include "../include/gui/Boardview/TileRenderer.hpp"
#include "../include/gui/MathCore/IsoTransformer.hpp"

PawnRenderer *PawnRenderer::instance = nullptr;

PawnRenderer::PawnRenderer(const std::string path)
{
    spriteSheets = LoadTexture(path.c_str());
    frameWidth = spriteSheets.width / 2;
    frameHeight = spriteSheets.height / 2;
}

PawnRenderer::~PawnRenderer()
{
    UnloadTexture(spriteSheets);
}

PawnRenderer &PawnRenderer::getInstance()
{
    if (instance == nullptr) {
        instance = new PawnRenderer("data/assets/players.png");
    }
    return *instance;
}

void PawnRenderer::DrawPawn(int tileIndex, int playerIndex)
{
    Vector2 tilepos = IsoTransformer::GetScreenPosition(tileIndex);
    int column = playerIndex % 2;
    int row = playerIndex / 2;

    /**
     *  [  ][  ]
     *  [  ][  ]
     *  Sprite berbentuk seperti ini misalnya index 4 akan menghasilkan 0, 1
     *  bagian bawah kaanan
     */

    Rectangle sourceRect = {
        (float)(column * frameWidth),
        (float)(row * frameHeight),
        (float)(frameWidth),
        (float)(frameHeight)};

    Vector2 playerOffset = {0, 0};
    // agar tidak saling bertumpukan
    switch (playerIndex) {
        case 0:
            playerOffset = {-10, -5};
            break;
        case 1:
            playerOffset = {10, -5};
            break;
        case 2:
            playerOffset = {-10, 5};
            break;
        case 3:
            playerOffset = {10, 5};
            break;
        default:
            // TODO throw exceprion
            break;
    }

    // Posisi akhir
    float displayWidth = 40.0f;
    float displayHeight = 40.0f;

    Rectangle destRect = {
        tilepos.x + playerOffset.x,
        tilepos.y + playerOffset.y,
        displayWidth,
        displayHeight};

    Vector2 origin = {displayWidth / 2.0f, displayHeight - 5.0f};

    Vector2 shadow = tilepos + playerOffset;
    DrawEllipse(shadow.x, shadow.y, 15, 7, ColorAlpha(ABU_SHADE, 0.3f));

    DrawTexturePro(spriteSheets, sourceRect, destRect, origin, 0.0f, WHITE);
    GameManager &gm = GameManager::getInstance();
    Player *current_player = gm.getPlayers().at(playerIndex);
    TileRenderer::DrawTextIsometric(current_player->getUsername(), {0, 67}, tilepos, 0);
}
