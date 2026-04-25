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
        logger.log(currentPlayer->getUsername(), StateLog::FUND_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " → Dapat M100 dari setiap pemain");
        *player -= 100;
        *currentPlayer += 100;
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
    logger.log(currentPlayer->getUsername(), StateLog::FUND_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " → Pergi ke stasiun terdekat");
    *currentPlayer -= 700;
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

        *currentPlayer -= 200;
        *player += 200;
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::FUND_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " → Bayar M200 kepada setiap pemain");
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
    int nearestDistance = static_cast<int>(board.getTiles().size()) + 1;

    for (Railroad *railroad : board.getAllRailroad())
    {
        if (railroad == nullptr)
        {
            continue;
        }

        int distance = railroad->getIndex() - currentIndex;
        if (distance <= 0)
        {
            distance += static_cast<int>(board.getTiles().size());
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
        logger.log(currentPlayer->getUsername(), StateLog::CHANCE_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " → Pergi ke stasiun terdekat (" + nearestStation->getName() + ")");
        currentPlayer->moveTo(nearestStation, true);
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
        logger.log(currentPlayer->getUsername(), StateLog::CHANCE_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " → Mundur 3 petak (" + destination->getName() + ")");
        currentPlayer->moveTo(destination, false);
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
    logger.log(currentPlayer->getUsername(), StateLog::CHANCE_CARD, "Mendarat di " + currentPlayer->getCurrentTile()->getName() + " → Masuk penjara");
    currentPlayer->moveTo(jailTile, false);
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

static int randomInt(int minValue, int maxValue)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(gen);
}

MoveCard::MoveCard() : MoveCard(randomInt(1, 12)) {}
MoveCard::MoveCard(int tileCount) : SkillCard("MOVE_CARD", "Maju sejumlah " + std::to_string(tileCount) + " petak"), tileCount(tileCount) {}
int MoveCard::getTileCount() const
{
    return tileCount;
}

void MoveCard::setTileCount(int value)
{
    tileCount = value;
}

void MoveCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    Tile *destination = GameManager::getInstance().getBoard().goToTile(*currentPlayer->getCurrentTile(), tileCount);
    if (destination != nullptr)
    {
        currentPlayer->moveTo(destination, true);
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai MoveCard → Maju sejauh " + to_string(tileCount) + " petak, mendarat di " + destination->getName() + " (" + destination->getCode() + ")");
    }
}

DiscountCard::DiscountCard() : DiscountCard(static_cast<float>(randomInt(10, 50))) {}
DiscountCard::DiscountCard(float discount) : SkillCard("DISCOUNT_CARD", "Semua properti mendapatkan diskon sebesar " + std::to_string(discount) + " persen"), discount(discount) {}

void DiscountCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer != nullptr)
    {
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai DiscountCard → Dapat diskon sebesar " + to_string(discount));
        currentPlayer->activateDiscount(discount);
    }
}

ShieldCard::ShieldCard() : SkillCard("SHIELD_CARD", "Melindungi dari tagihan sewa dan sanksi selama 1 giliran") {}
void ShieldCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer != nullptr)
    {
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai ShieldCard → Bebas tagihan dan sanksi selama 1 giliran");
        currentPlayer->activateShield();
    }
}

TeleportCard::TeleportCard() : SkillCard("TELEPORT_CARD", "Berpindah ke petak tujuan") {}

void TeleportCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    CommandHandler &handler = GameManager::getInstance().getCommandHandler();
    std::string targetTile = handler.askInput("Masukkan kode petak tujuan: ");

    Tile *destination = GameManager::getInstance().getBoard().getTile(targetTile);
    if (destination != nullptr)
    {   
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai TeleportCard → Pindah ke " + destination->getName() + " (" + destination->getCode() + ")");
        currentPlayer->moveTo(destination, false);
    } else {
        GameManager::getInstance().writeLine("Kode petak tidak valid.");
    }
}

LassoCard::LassoCard() : SkillCard("LASSO_CARD", "Tarik pemain di depan ke posisimu") {}

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
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai LassoCard → Pemain " + target->getUsername() + " pindah ke petak " + currentPlayer->getCurrentTile()->getName() + " (" + currentPlayer->getCurrentTile()->getCode() + ")");
        target->moveTo(currentPlayer->getCurrentTile(), false);
    }
}

DemolitionCard::DemolitionCard() : SkillCard("DEMOLITION_CARD", "Hancurkan properti lawan pada petak tujuan") {}

void DemolitionCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    CommandHandler &handler = GameManager::getInstance().getCommandHandler();
    std::string targetCode = handler.askInput("Masukkan kode properti target: ");

    Street *street = dynamic_cast<Street *>(GameManager::getInstance().getBoard().getTile(targetCode));
    if (street != nullptr && street->getOwner() != nullptr && street->getOwner() != currentPlayer && street->getCurrentLevel() > 0)
    {
        street->setCurrentLevel(street->getCurrentLevel() - 1);
        Logger &logger = Logger::getInstance();
        logger.log(currentPlayer->getUsername(), StateLog::SKILL_CARD, "Pakai DemolitionCard → Bangunan di " + street->getName() + " dihancurkan.");
        GameManager::getInstance().writeLine("Bangunan di " + street->getName() + " dihancurkan.");
    } else {
        GameManager::getInstance().writeLine("Target tidak valid atau tidak memiliki bangunan.");
    }
}
