#ifndef TOWN_H
#define TOWN_H

#include "Common.h"
#include "Creature.h"
#include <TFT_eSPI.h>
#include <Arduino.h>
#include "Explore.h"


class Town {
private:
    Creature* player;
    Explore* exploreSystem;  // Add the Explore attribute
     int prevPlayerGold = -1;
     unsigned long lastDebounceTime = 0;
     const unsigned long debounceDelay = 150;
public:
    Town(Creature* player);
    bool isAtShop;
    bool isAtInn;
    bool isExploring;
    bool isAtTown;

    void enterTown();
    void visitShop();
    void buyItems();
    void sellItems();

    // Include parameters in these method declarations to match the .cpp file
    void renderBuyMenu(int buyOption);
    void executePurchase(int buyOption);
    void renderSellMenu(int sellOption);
    void executeSale(int sellOption);
    void renderShopMenu(int shopOption);
    void renderTownMenu(int townOption); // If you need to track the selected option in the town menu
    void displayGold(int yPosition);
    void stayAtInn();
    void explore();
    void renderInnMenu(int innOption); // Include parameter if selection tracking is needed
    void restAtInn();
    void handleTownInput();
};

#endif
