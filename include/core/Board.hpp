#pragma once

#include <map>
#include <string>
#include <vector>

class Player;
class Tile;
class Railroad;
class Utility;

class Board {
private:
	std::vector<Tile*> tiles;
	std::map<int, Player*> playerPos;
	int tileCount;
	int goIndex;
	int jailIndex;

public:
	Board(int size);

	Tile* getTile(std::string code) const; // done
	Tile* getTile(int index) const; // done
	Tile* getJailTile() const; // done
	std::vector<Tile*> getTiles() const{return this->tiles;}; // done
	Tile* goToTile(Tile& current, int moveAmount) const; // done
	Tile* goToTile(const std::string&, int);
	void setGoIndex(int index){ this->goIndex = index; } // done
	void setJailIndex(int index){ this->jailIndex = index; } // done
	std::vector<Tile*> getColorGroup(const std::string& color) const ; // done
	std::vector<Railroad*> getAllRailroad() const ; // done
	std::vector<Utility*> getAllUtility() const ; // done
	bool canBuildHouse(Player &player, Tile* tile); // done
	int getRailroadLevel(Tile* tile);
	int getUtilityLevel(Tile* tile);
	Player* getNextPlayer(Player* player);
	int stringToIndex(const std::string&);
	void addTile(Tile* newTile); // done
};
