#include "../include/gui/Boardview/TileRenderer.hpp"
#include "../include/gui/UIOverlay/UIComponent.hpp"

namespace {

std::string money(int value)
{
    return "M" + std::to_string(value);
}

std::string propertyStatusLabel(PROPERTY_STATUS status)
{
    switch (status) {
    case BANK:
        return "BANK";
    case OWNED:
        return "OWNED";
    case MORTGAGED:
        return "MORTGAGED";
    default:
        return "UNKNOWN";
    }
}

std::string ownerLabel(Player* owner)
{
    if (owner == nullptr) {
        return "BANK";
    }
    return owner->getUsername();
}

std::string aktaStatusLabel(PROPERTY_STATUS status, Player* owner)
{
    if (status == BANK) {
        return "BANK";
    }

    return propertyStatusLabel(status) + " (" + ownerLabel(owner) + ")";
}

} // namespace

void TilePopup::renderPopUp(Street *s)
{
    if (s == nullptr) {
        return;
    }

    data_.setTitle("AKTA KEPEMILIKAN");
    data_.setSubtitle("[" + s->getColor() + "] " + s->getName() + " (" + s->getCode() + ")");
    data_.setDescription("Lihat rincian lengkap properti ini sebelum beli, bangun, atau gadai.");

    const int level = s->getCurrentLevel();
    std::string kondisiBangunan = "Tanah kosong";
    if (level >= 1 && level <= 4) {
        kondisiBangunan = std::to_string(level) + " rumah";
    } else if (level == 5) {
        kondisiBangunan = "Hotel";
    }

    data_.setFields({
        PopUpField("Harga Beli", money(s->getLandCost())),
        PopUpField("Nilai Gadai", money(s->getMortgageValue())),
        PopUpField("Sewa L0", money(s->getRentCostLevel(0))),
        PopUpField("Sewa L1", money(s->getRentCostLevel(1))),
        PopUpField("Sewa L2", money(s->getRentCostLevel(2))),
        PopUpField("Sewa L3", money(s->getRentCostLevel(3))),
        PopUpField("Sewa L4", money(s->getRentCostLevel(4))),
        PopUpField("Sewa Hotel", money(s->getRentCostLevel(5))),
        PopUpField("Harga Rumah", money(s->getHouseCost())),
        PopUpField("Harga Hotel", money(s->getHotelCost())),
        PopUpField("Status", aktaStatusLabel(s->getPropertyStatus(), s->getOwner()))
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(s->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

void TilePopup::renderPopUp(Railroad *r)
{
    if (r == nullptr) {
        return;
    }

    data_.setTitle("AKTA STASIUN");
    data_.setSubtitle("[" + r->getCode() + "] " + r->getName());
    data_.setDescription("Stasiun menambah kekuatan sewa berdasarkan jumlah stasiun yang dimiliki.");
    data_.setFields({
        PopUpField("Sewa Saat Ini", money(r->getRentCost())),
        PopUpField("Nilai Gadai", money(r->getMortgageValue())),
        PopUpField("Festival", "x" + std::to_string(r->getFestivalMultiplier())),
        PopUpField("Status", aktaStatusLabel(r->getPropertyStatus(), r->getOwner()))
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(r->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

void TilePopup::renderPopUp(Utility *u)
{
    if (u == nullptr) {
        return;
    }

    const int totalDice = GameManager::getInstance().getDice().getTotal();

    data_.setTitle("AKTA UTILITAS");
    data_.setSubtitle("[" + u->getCode() + "] " + u->getName());
    data_.setDescription("Sewa utilitas dihitung dari total dadu x pengali sesuai jumlah utilitas pemilik.");
    data_.setFields({
        PopUpField("Total Dadu", std::to_string(totalDice)),
        PopUpField("Sewa Saat Ini", money(u->getRentCost())),
        PopUpField("Nilai Gadai", money(u->getMortgageValue())),
        PopUpField("Festival", "x" + std::to_string(u->getFestivalMultiplier())),
        PopUpField("Status", aktaStatusLabel(u->getPropertyStatus(), u->getOwner()))
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(u->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

// Taxes
void TilePopup::renderPopUp(PBM *pbm)
{
    if (pbm == nullptr) {
        return;
    }

    data_.setTitle("PAJAK BARANG MEWAH");
    data_.setSubtitle("[" + pbm->getCode() + "] " + pbm->getName());
    data_.setDescription("Pajak langsung dipotong ke Bank saat mendarat.");
    data_.setFields({
        PopUpField("Nominal Pajak", money(pbm->getFixedTax())),
        PopUpField("Jenis", "Flat")
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(pbm->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

void TilePopup::renderPopUp(PPH *pph)
{
    if (pph == nullptr) {
        return;
    }

    data_.setTitle("PAJAK PENGHASILAN");
    data_.setSubtitle("[" + pph->getCode() + "] Pilih metode pembayaran");
    data_.setDescription("Sesuai aturan, pemain memilih dulu: flat atau persentase kekayaan.");
    data_.setFields({
        PopUpField("Opsi 1 (Flat)", money(pph->getFlatTax())),
        PopUpField("Opsi 2 (Persentase)", std::to_string(pph->getTaxPercentage()) + "% dari total kekayaan")
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(pph->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

// Special Tiles
void TilePopup::renderPopUp(CardTile *ct)
{
    if (ct == nullptr) {
        return;
    }

    std::string deckType = "Kartu Acak";
    if (ct->getCode() == "KSP") {
        deckType = "Kesempatan";
    } else if (ct->getCode() == "DNU") {
        deckType = "Dana Umum";
    }

    data_.setTitle("PETAK KARTU");
    data_.setSubtitle("[" + ct->getCode() + "] " + ct->getName());
    data_.setDescription("Ambil kartu teratas dan jalankan efeknya sekarang juga.");
    data_.setFields({
        PopUpField("Jenis Deck", deckType),
        PopUpField("Eksekusi", "Otomatis")
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(ct->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

void TilePopup::renderPopUp(Festival *f)
{
    if (f == nullptr) {
        return;
    }

    data_.setTitle("PETAK FESTIVAL");
    data_.setSubtitle("[" + f->getCode() + "] " + f->getName());
    data_.setDescription("Pilih properti milikmu untuk melipatgandakan sewa selama 3 giliran.");
    data_.setFields({
        PopUpField("Efek Awal", "Sewa x2"),
        PopUpField("Durasi", "3 giliran")
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(f->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

void TilePopup::renderPopUp(Go *g)
{
    if (g == nullptr) {
        return;
    }

    data_.setTitle("PETAK MULAI");
    data_.setSubtitle("Selamat datang di GO");
    data_.setDescription("Setiap berhenti atau melewati GO, pemain mendapat gaji dari Bank.");
    data_.setFields({
        PopUpField("Gaji GO", money(g->getPayment()))
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(g->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

void TilePopup::renderPopUp(Prison *p)
{
    if (p == nullptr) {
        return;
    }

    data_.setTitle("PETAK PENJARA");
    data_.setSubtitle("[" + p->getCode() + "] " + p->getName());
    data_.setDescription("Bisa sekadar mampir atau jadi tahanan tergantung cara tiba di petak ini.");
    data_.setFields({
        PopUpField("Denda Keluar", money(p->getFee()))
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(p->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

void TilePopup::renderPopUp(Trap *tr)
{
    if (tr == nullptr) {
        return;
    }

    data_.setTitle("PERGI KE PENJARA");
    data_.setSubtitle("[" + tr->getCode() + "] " + tr->getName());
    data_.setDescription("Mendarat di sini membuat pemain langsung dipindahkan ke penjara.");
    data_.setFields({
        PopUpField("Efek", "Langsung masuk penjara")
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(tr->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}

void TilePopup::renderPopUp(FreeParking *fp)
{
    if (fp == nullptr) {
        return;
    }

    data_.setTitle("BEBAS PARKIR");
    data_.setSubtitle("[" + fp->getCode() + "] " + fp->getName());
    data_.setDescription("Petak istirahat. Tidak ada aksi khusus saat berhenti di sini.");
    data_.setFields({
        PopUpField("Efek", "Tidak ada")
    });

    Tile *sTile = GameManager::getInstance().getBoard().getTile(fp->getIndex());
    bgColor = (sTile != nullptr) ? TileRenderer::ParseColor(sTile).first : BOARD_BASE;
    drawPopUp();
}