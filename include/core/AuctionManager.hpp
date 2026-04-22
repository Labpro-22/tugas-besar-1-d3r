#pragma once

#include <vector>

class Player;
class Property;

class AuctionManager {
private:
    std::vector<Player*> getAuctionOrder(const std::vector<Player*>& players, Player* triggerPlayer, Player* excludedPlayer) const;
    void transferProperty(Property* property, Player* winner, int price) const;

public:
    bool runAuction(Property* property, Player* excludedPlayer = nullptr) const;
};
