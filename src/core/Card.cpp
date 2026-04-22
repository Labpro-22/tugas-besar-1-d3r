#include "../../include/core/Card.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

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
    }
}

NearestStationCard::NearestStationCard() : AutoUseCard("NEAREST_STATION", "Pergi ke stasiun terdekat") {}
void NearestStationCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    // belum kepikiran
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

    currentPlayer->moveTo(jailTile, false);
    currentPlayer->setToJailed();
}

CARD_TYPE SkillCard::getCardType() const
{
    return SKILL;
}

SkillCard::SkillCard(const std::string &cardName, const std::string &cardDescription) : Card(cardName, cardDescription) {}
MoveCard::MoveCard(int moveTileMax) : SkillCard("MOVE_CARD", "Maju sejumlah" + to_string(moveTileMax) + "petak"), moveTileMax(moveTileMax) {}
int MoveCard::getMoveTileMax() const
{
    return moveTileMax;
}

void MoveCard::setMoveTileMax(int value)
{
    moveTileMax = value;
}

void MoveCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    Tile *destination = GameManager::getInstance().getBoard().goToTile(*currentPlayer->getCurrentTile(), moveTileMax);
    if (destination != nullptr)
    {
        currentPlayer->moveTo(destination, true);
    }
}

DiscountCard::DiscountCard(float discount) : SkillCard("DISCOUNT_CARD", "Semua properti mendapatkan diskon sebesar" + to_string(discount / 100) + "persen"), discount(discount) {}

void DiscountCard::useCard(Player *, std::vector<Player *>)
{
    // need finished player
}

ShieldCard::ShieldCard() : SkillCard("SHIELD_CARD", "Melindungi dari efek kartu lawan") {}

void ShieldCard::useCard(Player *, std::vector<Player *>)
{
    // need finished player
}

TeleportCard::TeleportCard() : SkillCard("TELEPORT_CARD", "Berpindah ke petak tujuan") {}

void TeleportCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    Tile *destination = nullptr;
    if (destination != nullptr)
    {
        currentPlayer->moveTo(destination, false);
    }
}

LassoCard::LassoCard() : SkillCard("LASSO_CARD", "Tarik pemain di depan ke posisimu") {}

void LassoCard::useCard(Player *currentPlayer, std::vector<Player *>)
{
    if (currentPlayer == nullptr || currentPlayer->getCurrentTile() == nullptr)
    {
        return;
    }

    const int currentIndex = currentPlayer->getCurrentTile()->getIndex();
    Player *target = GameManager::getInstance().getBoard().getNextPlayer(currentIndex);
    if (target != nullptr)
    {
        target->moveTo(currentPlayer->getCurrentTile(), false);
    }
}

DemolitionCard::DemolitionCard() : SkillCard("DEMOLITION_CARD", "Hancurkan properti lawan pada petak tujuan") {}

void DemolitionCard::useCard(Player *, std::vector<Player *>)
{
}
