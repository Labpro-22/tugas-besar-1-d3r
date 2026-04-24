#include "../../include/core/Tile.hpp"
#include "../../include/core/GameManager.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

Festival::Festival(int index, const string& code, const string& name, const string& color) : Tile(index, code, name, color) {}

void Festival::addMultiplier(const string& propertyCode) {
    GameManager& gm = GameManager::getInstance();
    Property* selected = nullptr;
    
    for (Tile* tile : gm.getBoard().getTiles()){
        Property* prop = dynamic_cast<Property*>(tile);
        if (prop && prop->getCode() == propertyCode) {
            selected = prop;
            break;
        }
    }
    
    if (!selected){
        return;
    }
    int currentMulti = selected->getFestivalMultiplier();
    if (currentMulti <= 0){ 
        currentMulti = 1;
    }
    int oldRent = selected->getRentCost();
    
    if (currentMulti == 1){
        selected->setFestivalMultiplier(2);
        selected->setFestivalDuration(3);
        gm.writeLine("");
        gm.writeLine("Efek festival aktif!");
        gm.writeLine("");
        gm.writeLine("Sewa awal: M" + to_string(oldRent));
        gm.writeLine("Sewa sekarang: M" + to_string(selected->getRentCost()));
        gm.writeLine("Durasi: 3 giliran");
    } else if (currentMulti < 8){
        selected->setFestivalMultiplier(currentMulti * 2);
        selected->setFestivalDuration(3);
        gm.writeLine("");
        gm.writeLine("Efek diperkuat!");
        gm.writeLine("");
        gm.writeLine("Sewa sebelumnya: M" + to_string(oldRent));
        gm.writeLine("Sewa sekarang: M" + to_string(selected->getRentCost()));
        gm.writeLine("Durasi di-reset menjadi: 3 giliran");
    } else{
        selected->setFestivalDuration(3);
        gm.writeLine("");
        gm.writeLine("Efek sudah maksimum (harga sewa sudah digandakan tiga kali)");
        gm.writeLine("");
        gm.writeLine("Durasi di-reset menjadi: 3 giliran");
    }
}

void Festival::runTile(Player* player) {
    if (!player){
        return;
    }
    GameManager& gm = GameManager::getInstance();
    gm.writeLine("Kamu mendarat di petak Festival!");
    gm.writeLine("");

    vector<Property*> props = player->getOwnedProperties();
    if (props.empty()){
        gm.writeLine("Kamu belum memiliki properti.");
        return;
    }

    gm.writeLine("Daftar properti milikmu:");
    for (Property* prop : props){
        gm.writeLine("- " + prop->getCode() + " (" + prop->getName() + ")");
    }
    gm.writeLine("");

    CommandHandler& cmd = gm.getCommandHandler();
    string selectedCode = "";

    while (true){
        selectedCode = cmd.askInput("Masukkan kode properti: ");
        
        bool found = false;
        for (Property* prop : props){
            if (prop->getCode() == selectedCode) {
                found = true;
                break;
            }
        }

        if (found) {
            break;
        } else {
            bool exists = false;
            for (Tile* t : gm.getBoard().getTiles()){
                if (t->getCode() == selectedCode){
                    exists = true;
                    break;
                }
            }
            if (exists){
                gm.writeLine("-> Properti bukan milikmu!");
            } else{
                gm.writeLine("-> Kode properti tidak valid!");
            }
            gm.writeLine("");
        }
    }

    addMultiplier(selectedCode);
}

