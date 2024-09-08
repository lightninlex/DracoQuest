#include "Equipment.h"
#include "Creature.h"
Equipment::Equipment(String name, int ATK, int DEF, int SPD, String equipmentType, int price)
    : Item(name, "equipment", price), ATK(ATK), DEF(DEF), SPD(SPD), equipmentType(equipmentType) {
    this->name = name;
    this->type = "equipment";
}

void Equipment::use(Creature& player) {
    player.ATK += this->ATK;
    player.DEF += this->DEF;
    player.SPD += this->SPD;
    this->isEquipped = true;
}

void Equipment::unequip(Creature& player) {
    if (isEquipped) {
        player.ATK -= this->ATK;
        player.DEF -= this->DEF;
        player.SPD -= this->SPD;
        this->isEquipped = false;
    }
}