#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Board.hpp"
#include "Card.hpp"
#include "Deck.hpp"
#include "Logger.hpp"
#include "Player.hpp"

class Tile;

class GameManager {
private:
	int turn;
	int maxTurn;
	int activePlayerCount;
	int playerCount;
	std::vector<Player*> players;
	Board board;
	CardDeck<SkillCard> deckSkill;
	CardDeck<AutoUseCard> deckChance;
	CardDeck<AutoUseCard> deckCurrency;
    Logger logger;
	Player* currentTurnPlayer;
	static std::vector<int> dice;

public:
	GameManager();
	static GameManager& getInstance() {
		static GameManager instance;
		return instance;
	}

	void setTurn(int turn) { this->turn = turn; };
	void setMaxTurn(int maxTurn) { this->maxTurn = maxTurn; };
	void setActivePlayerCount(int activePlayerCount) { this->activePlayerCount = activePlayerCount; };
	void setPlayerCount(int playerCount) { this->playerCount = playerCount; };
	void setPlayers(std::vector<Player*> players) { this->players = players; };
	void setTiles(Board tiles) { this->board = tiles; };
	void setDeckSkill(CardDeck<SkillCard> deckSkill) { this->deckSkill = std::move(deckSkill); };
	void setDeckChance(CardDeck<AutoUseCard> deckChance) { this->deckChance = std::move(deckChance); };
	void setDeckCurrency(CardDeck<AutoUseCard> deckCurrency) { this->deckCurrency = std::move(deckCurrency); };
	void setLogger(Logger logger) { this->logger = logger; };
	void setCurrentTurnPlayer(Player* currentTurnPlayer) { this->currentTurnPlayer = currentTurnPlayer; };
	static void setDice(std::vector<int> dice) { GameManager::dice = dice; };
	void setAllPlayersCurrency(int currency) {
		for (Player* player : players) {
			if (player != nullptr) {
				player->setCurrency(currency);
			}
		}
	}
	void addTile(Tile* tile) {
	    board.addTile(tile);
	}	

	bool isGameValid();
	void runGame();
	void auction(Tile*);
	void initBoard();
	void initPlayers();
	void initStateLogs();
	void initSkillDeck();
	Board& getBoard() {
		return board;
	}
	const Board& getBoard() const {
		return board;
	}
    Logger& getLogger();
};
