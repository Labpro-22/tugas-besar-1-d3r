#pragma once

#include <memory>

class PlayerHUD;
class ActionPanel;
class PopupDialog;
struct RenderConfig;

class UIManager {
private:
    std::unique_ptr<PlayerHUD> playerHUD;
    std::unique_ptr<ActionPanel> actionPanel;
    std::unique_ptr<PopupDialog> popupDialog;
    RenderConfig* renderConfig;
    
public:
    UIManager(RenderConfig* config);
    
    void initialize();
    void render();
    void update(float deltaTime);
    
    PlayerHUD* getPlayerHUD();
    ActionPanel* getActionPanel();
    PopupDialog* getPopupDialog();
    
    void showPopup(const std::string& title, const std::string& message);
    void hidePopup();
    
    ~UIManager();
};
