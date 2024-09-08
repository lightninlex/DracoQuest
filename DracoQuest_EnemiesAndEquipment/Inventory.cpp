#include "Inventory.h"
#include "Creature.h"
#include "Equipment.h"
#include "Common.h"
Inventory::Inventory() : itemCount(0) {}

void Inventory::useItem(int index, Creature& target) {
  int messageY = 0; // Adjust this value to match where resolveCombat() prints its messages
    if (index < 0 || index >= itemCount) {
        return;
    }
    Item* item = items[index];
    if (item->isConsumable()) {
        item->use(target);
        removeItem(index);
    } else if (item->isEquipment()) {
        Equipment* equipment = static_cast<Equipment*>(item);
        if (equipment->isEquipped) {
            equipment->unequip(target);  // You'll need to implement this method
            tft.fillRect(0, messageY, 320, 40, TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            String message = "You removed your " + item->name + ".";
            tft.drawString(message, 10, messageY, 1); // You might need to adjust the X position (10) and font size (2) as necessary
            delay(800);
            tft.fillRect(0, messageY, 320, 40, TFT_BLACK);
        } else {
            equipment->use(target);  // Equips the item
            tft.fillRect(0, messageY, 320, 40, TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            String message = "You have equipped your " + item->name + "!";
            tft.drawString(message, 10, messageY, 1); // You might need to adjust the X position (10) and font size (2) as necessary
            delay(800);
            tft.fillRect(0, messageY, 320, 40, TFT_BLACK);
        }
    }
}


void Inventory::removeItem(int index) {
    if (index < 0 || index >= itemCount) {
        return; // Index out of bounds
    }

    delete items[index];  // Free the memory if dynamically allocated
    for (int i = index; i < itemCount - 1; i++) {
        items[i] = items[i + 1];
    }
    itemCount--;
}



int Inventory::addItem(Item* item) {
    if (itemCount < MAX_ITEMS) {
        items[itemCount] = item;
        return itemCount++;
    }
    return -1;
}

void Inventory::equipItem(int index, Creature& target) {

    if (items[index]->isEquipment() && !static_cast<Equipment*>(items[index])->isEquipped) {
        Equipment* equipment = static_cast<Equipment*>(items[index]);
        equipment->use(target);  // This will update the player's stats
        equipment->isEquipped = true;
    } 
}
