#pragma once

#include <vector>

struct RenderConfig;

class DiceRenderer {
private:
    RenderConfig* renderConfig;
    std::vector<int> diceValues;
    bool isRolling = false;
    float rollDuration = 0.0f;
    
public:
    DiceRenderer(RenderConfig* config);
    
    void render();
    void renderDice(int index, int value);
    void startRolling(float duration);
    void updateRolling(float deltaTime);
    void setDiceValues(const std::vector<int>& values);
    
    std::vector<int> getDiceValues() const;
    bool getIsRolling() const;
};
