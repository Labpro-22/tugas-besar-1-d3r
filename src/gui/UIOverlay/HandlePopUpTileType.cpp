#include "../include/gui/Boardview/TileRenderer.hpp"
#include "../include/gui/UIOverlay/UIComponent.hpp"
#include <iostream>

void TilePopup::renderPopUp(Street *s)
{
    data_.setTitle("AKTA PEMILIKAN");
    data_.setSubtitle("[" + s->getColor() + "] " + s->getName() + "(" + s->getCode() + ")");
    data_.setDescription("");
    data_.setFields({
        PopUpField("Harga Beli", std::to_string(s->getLandCost())),
        PopUpField("Harga Gadai", std::to_string(s->getMortgageValue())),
        PopUpField("Sewa", std::to_string(s->getRentCost()))
    });
    Tile *sTile = GameManager::getInstance().getBoard().getTile(s->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};
void TilePopup::renderPopUp(Railroad *r)
{
    data_.setTitle("Kamu datang di " + r->getName());
    data_.setSubtitle("");
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(r->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};
void TilePopup::renderPopUp(Utility *u)
{
    data_.setTitle("Kamu datang di " + u->getName());
    data_.setSubtitle("");
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(u->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};

// Taxes
void TilePopup::renderPopUp(PBM *pbm)
{
    data_.setTitle("Kamu datang di " + pbm->getName());
    data_.setSubtitle("");
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(pbm->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};
void TilePopup::renderPopUp(PPH *pph)
{
    data_.setTitle("Kamu datang di " + pph->getName());
    data_.setSubtitle("Kamu harus bayar pajak");
    data_.setDescription("");
    data_.setFields({
        PopUpField("1. Bayar flat M", std::to_string(pph->getFlatTax())),
        PopUpField("2. Bayar " + std::to_string(pph->getTaxPercentage()) + "dari total kekayaan", ""),
    });
    Tile *sTile = GameManager::getInstance().getBoard().getTile(pph->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};

// Special Tiles
void TilePopup::renderPopUp(CardTile *ct)
{
    data_.setTitle("Kamu datang di " + ct->getName());
    data_.setSubtitle("");
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(ct->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};
void TilePopup::renderPopUp(Festival *f)
{
    data_.setTitle("Kamu datang di " + f->getName());
    data_.setSubtitle("");
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(f->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};
void TilePopup::renderPopUp(Go *g)
{
    data_.setTitle("Kamu Baru Mulai");
    data_.setSubtitle("Kamu mendapatkan M" + std::to_string(g->getPayment()));
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(g->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};
void TilePopup::renderPopUp(Prison *p)
{
    data_.setTitle("Kamu datang di " + p->getName());
    data_.setSubtitle("");
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(p->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};
void TilePopup::renderPopUp(Trap *tr)
{
    data_.setTitle("Kamu datang di " + tr->getName());
    data_.setSubtitle("");
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(tr->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};
void TilePopup::renderPopUp(FreeParking *fp)
{
    data_.setTitle("Kamu datang di " + fp->getName());
    data_.setSubtitle("");
    data_.setDescription("");
    data_.setFields({});
    Tile *sTile = GameManager::getInstance().getBoard().getTile(fp->getIndex());
    bgColor = TileRenderer::ParseColor(sTile).first;
    drawPopUp();
};