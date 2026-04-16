#pragma once

#include <map>
#include <string>
#include <vector>

class Player;
class Tile;

class Board {
private:
	static std::vector<Tile*> tiles;
	static std::map<int, Player*> playerPos;
	static int tileCount;
	static int goIndex;
	static int jailIndex;

public:
	Board() = default;

	static Tile* getTile(int);
	static Tile* getJailTile();
	static Tile* goToTile(Tile&, int);
	static std::vector<Tile*> getColorGroup(const std::string&);
	static bool canBuildHouse(Player, Tile*);
	static int getRailroadLevel(Tile*);
	static int getUtilityLevel(Tile*);
	static Player* getNextPlayer(int currentIndex);
	static int stringToIndex(const std::string&);
	static Tile* goToTile(const std::string&, int);
	static void addTile(Tile*);
};
