#pragma once

#include <string>
#include <functional>

struct RenderConfig;

class PopupDialog {
private:
    RenderConfig* renderConfig;
    std::string title;
    std::string message;
    bool isVisible = false;
    
    std::function<void()> onConfirm;
    std::function<void()> onCancel;
    
public:
    PopupDialog(RenderConfig* config);
    
    void show(const std::string& title, const std::string& message);
    void hide();
    
    void render();
    void handleInput();
    
    void setConfirmCallback(std::function<void()> callback);
    void setCancelCallback(std::function<void()> callback);
    
    bool getIsVisible() const;
    
    ~PopupDialog();
};
