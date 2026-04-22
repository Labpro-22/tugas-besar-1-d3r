#pragma once

struct RenderConfig;

class CenterRenderer {
private:
    RenderConfig* renderConfig;
    
public:
    CenterRenderer(RenderConfig* config);
    
    void render();
    void renderDiceInfo();
    void renderGameInfo();
    void renderCardInfo();
};
