#pragma once

#include <string>
#include <vector>

class Player;
struct RenderConfig;

class PlayerHUD {
private:
    Player* currentPlayer;
    RenderConfig* renderConfig;
    std::vector<Player*> allPlayers;
    
public:
    PlayerHUD(RenderConfig* config);
    
    void setCurrentPlayer(Player* player);
    void setAllPlayers(const std::vector<Player*>& players);
    
    void render();
    void renderCurrentPlayerInfo();
    void renderPlayerStats();
    void renderBalance();
    void renderCards();
    
    void update();
    
    ~PlayerHUD();
};
