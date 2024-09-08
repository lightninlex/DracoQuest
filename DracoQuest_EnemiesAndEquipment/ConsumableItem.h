#ifndef CONSUMABLEITEM_H
#define CONSUMABLEITEM_H

#include "Item.h"
#include "Creature.h"

class ConsumableItem : public Item {
public:
    int potency;
    String effectType;
    bool countTime = false;
    bool isConsumable() override { return true; }
    ConsumableItem(String name, int potency, String effectType, int price);
    int getSellValue() const override {
        return potency / 2;  // Example calculation, adjust as needed
    }
    void use(Creature& target) override;
};

#endif // CONSUMABLEITEM_H
