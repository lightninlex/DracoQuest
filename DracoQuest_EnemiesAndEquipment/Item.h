#ifndef ITEM_H
#define ITEM_H
#include <Arduino.h>
class Creature;
class Item {
public:
    String name;
    String type;
    int price;  // Add a price member
    Item(String name, String type, int price) : name(name), type(type), price(price) {}
    virtual int getSellValue() const {
        // Assuming you have a 'price' member in Item class
        return price / 2;  // Just an example: sell value is half of buy price
    }
    virtual void use(Creature& target) = 0;
    virtual bool isEquipment() const { return false; }
    virtual ~Item() {}
    virtual bool isConsumable() { return false; }
};
#endif
