#include "../../include/core/Card.hpp"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"
#include "../../include/core/Logger.hpp"

Card::Card(const std::string &cardName, const std::string &cardDescription)
    : cardName(cardName), cardDescription(cardDescription) {}

std::string Card::getCardName() const
{
    return cardName;
}

std::string Card::getCardDescription() const
{
    return cardDescription;
}

AutoUseCard::AutoUseCard(const std::string &cardName, const std::string &cardDescription)
    : Card(cardName, cardDescription), cardType(AUTO_USE) {}

CARD_TYPE AutoUseCard::getCardType() const
{
    return cardType;
}

BirthDayCard::BirthDayCard() : AutoUseCard("BIRTHDAY_CARD", "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain") {}
void BirthDayCard::useCard(Player *currentPlayer, std::vector<Player *> players)
{
    if (currentPlayer == nullptr)
    {
        return;
    }

    for (Player *player : players)
    {
        if (player == nullptr || player == currentPlayer)
        {
            continue;
        }

        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::FUND_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " Dapat M100 dari setiap pemain");
        GameManager::getInstance().pay(player, 100, currentPlayer);
    }
}

DoctorCard::DoctorCard() : AutoUseCard("DOCTOR_CARD", "Biaya dokter. Bayar M700") {}
void DoctorCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr)
    {
        return;
    }

    Logger &logger = Logger::getInstance();
    logger.log(currentPlayer->getUsername(), StateLog::FUND_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " Pergi ke stasiun terdekat");
    GameManager::getInstance().pay(currentPlayer, 700, nullptr);
}

CampaignCard::CampaignCard() : AutoUseCard("CAMPAIGN", "Anda mau nyaleg. Bayar M200 kepada setiap pemain") {}
void CampaignCard::useCard(Player *currentPlayer, std::vector<Player *> players)
{
    if (currentPlayer == nullptr)
    {
        return;
    }

    for (Player *player : players)
    {
        if (player == nullptr || player == currentPlayer)
        {
            continue;
        }

        GameManager::getInstance().pay(currentPlayer, 200, player);
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::FUND_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " Bayar M200 kepada setiap pemain");
    }
}

NearestStationCard::NearestStationCard() : AutoUseCard("NEAREST_STATION", "Pergi ke stasiun terdekat") {}
void NearestStationCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    Board &board = GameManager::getInstance().getBoard();
    const int currentIndex = currentPlayer->getCurrentTile()->getIndex();
    Tile *nearestStation = nullptr;
    const int boardOffset = (!board.getTiles().empty() && board.getTiles()[0] == nullptr) ? 1 : 0;
    const int boardSize = static_cast<int>(board.getTiles().size()) - boardOffset;
    int nearestDistance = boardSize + 1;

    for (Railroad *railroad : board.getAllRailroad())
    {
        if (railroad == nullptr)
        {
            continue;
        }

        int distance = railroad->getIndex() - currentIndex;
        if (distance <= 0)
        {
            distance += boardSize;
        }

        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearestStation = railroad;
        }
    }

    if (nearestStation != nullptr)
    {
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::CHANCE_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " Pergi ke stasiun terdekat (" + nearestStation->getName() + ")");
        currentPlayer->moveTo(nearestStation, true, FORWARD);
    }
}

MoveBackCard::MoveBackCard() : AutoUseCard("MOVE_BACK", "Mundur 3 petak") {}
void MoveBackCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    Tile *destination = GameManager::getInstance().getBoard().goToTile(*currentPlayer->getCurrentTile(), -3);

    if (destination != nullptr)
    {
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::CHANCE_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " Mundur 3 petak (" + destination->getName() + ")");
        currentPlayer->moveTo(destination, false, BACKWARD);
    }
}

ToJailCard::ToJailCard() : AutoUseCard("GO_TO_JAIL", "Masuk penjara") {}
void ToJailCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr)
    {
        return;
    }

    Tile *jailTile = GameManager::getInstance().getBoard().getJailTile();

    if (jailTile == nullptr)
    {
        return;
    }

    Logger &logger = Logger::getInstance();
    logger.log(currentPlayer->getUsername(), StateLog::CHANCE_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " Masuk penjara");
    currentPlayer->moveTo(jailTile, false, FORWARD);
    currentPlayer->setToJailed();
}

CARD_TYPE SkillCard::getCardType() const
{
    return SKILL;
}

SkillCard::SkillCard(const std::string &cardName, const std::string &cardDescription, const int cardValue, const int cardDuration) : Card(cardName, cardDescription), cardValue(cardValue), cardDuration(cardDuration) {}

int SkillCard::getCardValue() const {
    return cardValue;
}
int SkillCard::getCardDuration() const {
    return cardDuration;
}

void SkillCard::setCardValue(int value) {
    cardValue = value;
}
void SkillCard::setCardDuration(int duration) {
    cardDuration = duration;
}

static int randomInt(int minValue, int maxValue)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(gen);
}

MoveCard::MoveCard() : MoveCard(randomInt(1, 12)) {}
MoveCard::MoveCard(int tileCount) : SkillCard("MOVE_CARD", "Maju sejumlah " + std::to_string(tileCount) + " petak", tileCount, 0) {}
int MoveCard::getTileCount() const
{
    return getCardValue();
}

void MoveCard::setTileCount(int value)
{
    cardValue = value;
}

void MoveCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    Tile *destination = GameManager::getInstance().getBoard().goToTile(*currentPlayer->getCurrentTile(), getTileCount());
    if (destination != nullptr)
    {
        currentPlayer->moveTo(destination, true, FORWARD);
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai MoveCard -> Maju sejauh " + to_string(getTileCount()) + " petak, mendarat di " + destination->getName() + " (" + destination->getCode() + ")");
    }
}

DiscountCard::DiscountCard() : DiscountCard((randomInt(10, 50))) {}
DiscountCard::DiscountCard(int discount) : SkillCard("DISCOUNT_CARD", "Semua properti mendapatkan diskon sebesar " + std::to_string(discount) + " persen", discount, 1) {}

float DiscountCard::getDiscount() const {
    return static_cast<float>(getCardValue());
}
void DiscountCard::setDiscount(int discount) {
    setCardValue(discount);
}

void DiscountCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer != nullptr)
    {
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai DiscountCard -> Dapat diskon sebesar " + to_string(getDiscount()));
        currentPlayer->activateDiscount(getDiscount());
    }
}

ShieldCard::ShieldCard() : SkillCard("SHIELD_CARD", "Melindungi dari tagihan sewa dan sanksi selama 1 giliran", 0, 1) {}
void ShieldCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer != nullptr)
    {
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai ShieldCard -> Bebas tagihan dan sanksi selama 1 giliran");
        currentPlayer->activateShield();
    }
}

TeleportCard::TeleportCard() : SkillCard("TELEPORT_CARD", "Berpindah ke petak tujuan", 0 , 0) {}

void TeleportCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    CommandHandler &handler = GameManager::getInstance().getCommandHandler();
    std::string targetTile = handler.askInput("Masukkan kode petak tujuan: ");

    try {
        Tile *destination = GameManager::getInstance().getBoard().getTile(targetTile);
        if (destination == nullptr)
        {
            throw InvalidTileCodeException(targetTile);
        }

        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai TeleportCard -> Pindah ke " + destination->getName() + " (" + destination->getCode() + ")");
        currentPlayer->moveTo(destination, false, FORWARD);
    } catch (const InvalidTileCodeException&) {
        GameManager::getInstance().writeLine("Kode petak tidak valid.");
    }
}

LassoCard::LassoCard() : SkillCard("LASSO_CARD", "Tarik pemain di depan ke posisimu", 0, 0) {}

void LassoCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    Player *target = GameManager::getInstance().getBoard().getNextPlayer(currentPlayer);
    if (target != nullptr)
    {
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai LassoCard -> Pemain " + target->getUsername() + " pindah ke petak " + currentPlayer->getCurrentTile()->getName() + " (" + currentPlayer->getCurrentTile()->getCode() + ")");
        target->moveTo(currentPlayer->getCurrentTile(), false, FORWARD);
    }
}

DemolitionCard::DemolitionCard() : SkillCard("DEMOLITION_CARD", "Hancurkan properti lawan pada petak tujuan", 0, 0) {}

void DemolitionCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    CommandHandler &handler = GameManager::getInstance().getCommandHandler();
    std::string targetCode = handler.askInput("Masukkan kode properti target: ");

    try {
        Tile *targetTile = GameManager::getInstance().getBoard().getTile(targetCode);
        if (targetTile == nullptr)
        {
            throw InvalidTileCodeException(targetCode);
        }

        Street *street = dynamic_cast<Street *>(targetTile);
        if (street != nullptr && street->getOwner() != nullptr && street->getOwner() != currentPlayer && street->getCurrentLevel() > 0)
        {
            street->setCurrentLevel(street->getCurrentLevel() - 1);
            Logger &logger = Logger::getInstance();
            logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai DemolitionCard Bangunan di " + street->getName() + " dihancurkan.");
            GameManager::getInstance().writeLine("Bangunan di " + street->getName() + " dihancurkan.");
        } else {
            GameManager::getInstance().writeLine("Target tidak valid atau tidak memiliki bangunan.");
        }
    } catch (const InvalidTileCodeException&) {
        GameManager::getInstance().writeLine("Target tidak valid atau tidak memiliki bangunan.");
    }
}

FreeJailCard::FreeJailCard() : SkillCard("Bebas Penjara", "Bebas dari Penjara", 0, 0) {}

void FreeJailCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr)
    {
        return;
    }

    if (currentPlayer->getStatus() == JAILED) {
        Tile *jailTile = GameManager::getInstance().getBoard().getJailTile();
        if (jailTile != nullptr) {
            Prison* prison = dynamic_cast<Prison*>(jailTile);
            if (prison != nullptr) {
                prison->freeFromJailed(currentPlayer);
                Logger &logger = Logger::getInstance();
                logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai FreeJailCard → Bebas dari penjara");
                GameManager::getInstance().writeLine("Berhasil menggunakan Kartu Bebas Penjara. Kamu sekarang bebas!");
            }
        }
    } else {
        GameManager::getInstance().writeLine("Kartu ini hanya bisa digunakan saat berada di penjara.");
        // Should we refund the card? Usually skill cards are consumed, but we can assume the player is smart enough.
    }
}
