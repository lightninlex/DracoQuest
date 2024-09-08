#ifndef INVENTORY_H
#define INVENTORY_H
#include "Item.h"
class Creature;
class Inventory {
public:
    static const int MAX_ITEMS = 100;
    Item* items[MAX_ITEMS];
    int itemCount;
    Inventory();
    int addItem(Item* item);
    void useItem(int index, Creature& target);
    void equipItem(int index, Creature& target);
    void removeItem(int index);
};
#endif
