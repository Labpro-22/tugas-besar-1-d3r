#pragma once

#include <vector>
#include <utility>

class Player;
struct RenderConfig;

class PawnRenderer {
private:
    std::vector<Player*> players;
    RenderConfig* renderConfig;
    
public:
    PawnRenderer(RenderConfig* config);
    
    void addPlayer(Player* player);
    void render();
    void renderPawn(const Player* player);
    void animateMovement(Player* player, int targetTileIndex, float duration);
    
    std::pair<int, int> calculatePawnPosition(const Player* player) const;
    
    ~PawnRenderer();
};
