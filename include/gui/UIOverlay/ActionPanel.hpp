#pragma once

#include <string>
#include <vector>
#include <functional>

struct RenderConfig;

class ActionPanel {
private:
    RenderConfig* renderConfig;
    std::vector<std::pair<std::string, std::function<void()>>> actions;
    int selectedActionIndex = -1;
    bool isVisible = true;
    
public:
    ActionPanel(RenderConfig* config);
    
    void render();
    void addAction(const std::string& label, std::function<void()> callback);
    void clearActions();
    
    void selectAction(int index);
    void executeSelectedAction();
    
    void show();
    void hide();
    bool getIsVisible() const;
    
    int getSelectedActionIndex() const;
    int getActionCount() const;
    
    ~ActionPanel();
};
