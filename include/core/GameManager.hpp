#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "AuctionManager.hpp"
#include "Board.hpp"
#include "Card.hpp"
#include "CommandHandler.hpp"
#include "Deck.hpp"
#include "Dice.hpp"
#include "Logger.hpp"
#include "Player.hpp"

class Tile;
class CardTile;

class GameManager {
private:
	int turn;
	int maxTurn;
	int activePlayerCount;
	int playerCount;
	int initialCurrency;
	int doubleCount;
	std::vector<Player*> players;
	Board board;
	CommandHandler commandHandler;
	AuctionManager auctionManager;
	CardDeck<SkillCard> deckSkill;
	CardDeck<AutoUseCard> deckChance;
	CardDeck<AutoUseCard> deckCurrency;
	Logger logger;
	Player* currentTurnPlayer;
	Dice dice;
	bool useGuiStream;
	std::function<void(const std::string&)> outputCallback;
	std::function<std::string(const std::string&)> inputCallback;

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
	void setCurrentTurnPlayer(Player* currentTurnPlayer) { this->currentTurnPlayer = currentTurnPlayer; };
	static void setDice(std::vector<int> dice) { GameManager::getInstance().dice.setValues(dice); };
	void setUseGuiStream(bool useGuiStream) { this->useGuiStream = useGuiStream; }
	void setOutputCallback(std::function<void(const std::string&)> callback) { outputCallback = std::move(callback); }
	void setInputCallback(std::function<std::string(const std::string&)> callback) { inputCallback = std::move(callback); }
	void setAllPlayersCurrency(int currency) {
		initialCurrency = currency;
		for (Player* player : players) {
			if (player != nullptr) {
				player->setCurrency(currency);
			}
		}
	}
	void addTile(Tile* tile) {
	    board.addTile(tile);
	}	

	int getTurn() const { return turn; }
	int getMaxTurn() const { return maxTurn; }
	Player* getCurrentTurnPlayer() const { return currentTurnPlayer; }
	bool isGuiStreamActive() const { return useGuiStream; }
	Dice& getDice() { return dice; }
	const Dice& getDice() const { return dice; }
	CardDeck<SkillCard>& getSkillDeck() { return deckSkill; }
	const CardDeck<SkillCard>& getSkillDeck() const { return deckSkill; }
	CardDeck<AutoUseCard>& getChanceDeck() { return deckChance; }
	const CardDeck<AutoUseCard>& getChanceDeck() const { return deckChance; }
	CardDeck<AutoUseCard>& getCurrencyDeck() { return deckCurrency; }
	const CardDeck<AutoUseCard>& getCurrencyDeck() const { return deckCurrency; }
	std::vector<Player*>& getPlayers() { return players; }
	const std::vector<Player*>& getPlayers() const { return players; }
	CommandHandler& getCommandHandler() { return commandHandler; }
	AuctionManager& getAuctionManager() { return auctionManager; }
	bool isGameValid();
	void runGame();
	void auction(Tile*);
	void write(const std::string& text) const;
	void writeLine(const std::string& text) const;
	std::string readLine(const std::string& prompt) const;
	void rollDice();
	void rollDice(int dice1, int dice2);
	void initAutoUseDecks();
	void drawSkillCard(Player* player);
	void nextTurn();
	void printTurnInfo();
	void promptForCommand();
	bool isGameFinished() const;
	Player* getWinner() const;
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
	std::vector<Player*> getPlayer() const { return players; }
    Logger& getLogger();
	void pay(Player *debtor, int amount, Player* creditor);
	void sellPropertyToBank(Player* player, Property* property);
	void handleBankruptcy(Player *debtor, int amount, Player* creditor);
	void assetAcquisition(Player* debtor, Player* creditor);
};
