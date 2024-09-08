#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "Item.h"
#include "Creature.h"

class Equipment : public Item {
public:
    int ATK, DEF, SPD;
    String equipmentType;
    bool isEquipped = false; // Add this member variable

    Equipment(String name, int ATK, int DEF, int SPD, String equipmentType, int price);
    int getSellValue() const override {
        return (ATK + DEF + SPD)*2;  // Example calculation, adjust based on your game logic
    }
    void use(Creature& target) override;
    bool isEquipment() const override { return true; }
    void unequip(Creature& target);
};

#endif
