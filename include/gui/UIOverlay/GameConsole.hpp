#include "raylib.h"
#include <vector>
#include <string>
#include <functional>

class GameConsole
{
private:
    // Konfigurasi Area
    Rectangle bounds;
    Rectangle resizer;             
    Rectangle titleBar;            
    int fontSize;
    u_int maxHistory;
    
    // Data Teks
    std::vector<std::string> history;
    std::string currentInput;
    int scrollOffset = 0;
    bool waitingForBlockingInput = false;
    bool blockingInputReady = false;
    std::string blockingInputResult;

    // State Input
    bool isActive = true;
    int framesCounter = 0;
    
    // Resizing State
    bool isResizing = false;
    bool isMoving = false;        
    Vector2 lastMouse = {0.0f, 0.0f};
    const float minWidth = 200.0f;
    const float minHeight = 150.0f;
    const float maxWidth = 1000.0f;
    const float maxHeight = 700.0f;
    const float titleBarHeight = 25.0f;

    std::function<void(std::string)> commandCallback = nullptr;
    
public:
    GameConsole(Rectangle area, int fSize = 12);
    void WriteLine(std::string text);
    std::string ReadLineBlocking(const std::string& prompt);
    void ProcessCommand(std::string cmd) ;
    void AutoScrollToBottom();
    std::string GetFullHistoryText();
    void Update();
    void Render();
    void HandleScroll(float wheelValue);
    void disableConsole(){
        this->isActive = false;
    }

    void SetCommandCallback(std::function<void(std::string)> callback) {
        commandCallback = callback;
    }

    Rectangle getBounds(){return bounds;};
    ~GameConsole();
};

