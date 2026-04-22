#pragma once

#include <map>
#include <string>
#include <vector>

class Player;
class Tile;

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
	Tile* getTile(int index) const; 
	Tile* getJailTile() const;
	std::vector<Tile*> getTiles() const{return this->tiles;};
	Tile* goToTile(Tile& current, int moveAmount) const;
	void setGoIndex(int index){ this->goIndex = index; }
	void setJailIndex(int index){ this->jailIndex = index; }
	std::vector<Tile*> getColorGroup(const std::string& color) const ;
	bool canBuildHouse(Player, Tile*);
	int getRailroadLevel(Tile*);
	int getUtilityLevel(Tile*);
	Player* getNextPlayer(int currentIndex);
	int stringToIndex(const std::string&);
	Tile* goToTile(const std::string&, int);
	void addTile(Tile* newTile); // done
};