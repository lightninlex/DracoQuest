#include "Town.h"
#include "Equipment.h"
#include "ConsumableItem.h"

// modify to consider time of day, modify inn to restore energy
// shop prices should reflect the day of week and account for disposition, sales on SAT and SUN
// each tier of shop should gain an additional secret item index that is unlockable by higher disposition
// separate shops for the sake of disposition and variance, shopping right at opening or right before closing lowers disposition
// add tavern for npc's, sidequests, and minigames
// minigames: Hi Low...etc
// add bounty system
//add bank
const int UP = 32;
const int DOWN = 33;
const int LEFT = 25;
const int RIGHT = 26;
const int BACK = 27;
const int SELECT = 14;

Town::Town(Creature* player) : player(player) {
  exploreSystem = new Explore(player);  // Initialize exploration system
    isAtTown = false;
    isAtShop = false;
    isAtInn = false;
    isExploring = false;
}

void Town::enterTown() {
  tft.fillScreen(TFT_BLACK);
  int townOption = 0; // Initialize the option variable
    isAtTown = true;
    renderTownMenu(townOption);
}

void Town::visitShop() {
    isAtTown = false;
    int shopOption = -1;
    renderShopMenu(shopOption);
    while (isAtShop) {
        unsigned long currentMillis = millis();
        if (digitalRead(RIGHT) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            lastDebounceTime = currentMillis;
            shopOption = (shopOption + 1) % 3;
            renderShopMenu(shopOption);
        } else if (digitalRead(LEFT) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            lastDebounceTime = currentMillis;
            shopOption = (shopOption - 1) % 3;
            renderShopMenu(shopOption);
        }
        if (digitalRead(BACK) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            isAtShop = false;
            enterTown();
            break;
        }

        if (digitalRead(SELECT) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            lastDebounceTime = currentMillis;
            switch (shopOption) {
                case 0:
                    delay(300);
                    buyItems();
                    break;
                case 1:
                    delay(300);
                    sellItems();
                    break;
                case 2:
                    delay(300);
                    isAtShop = false;
                    enterTown();
                    break;
            }
        }
    }
}

void Town::buyItems() {
  const int numItems = 6;
    int buyOption = -1;
    renderBuyMenu(buyOption);

    while (isAtShop && !isAtTown) {
      unsigned long currentMillis = millis();
        if (digitalRead(DOWN) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            buyOption = (buyOption + 1) % (numItems + 1); // Assuming numItems is the count of items for sale
            renderBuyMenu(buyOption);
            lastDebounceTime = currentMillis;
        } else if (digitalRead(UP) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            buyOption = (buyOption - 1) % (numItems + 1); // Assuming numItems is the count of items for sale
            renderBuyMenu(buyOption);
            lastDebounceTime = currentMillis;
        }
        if (digitalRead(BACK) == LOW && currentMillis - lastDebounceTime - debounceDelay) {
          lastDebounceTime = currentMillis;
          isAtShop = false;
          enterTown();
          return;
        }
        if (digitalRead(SELECT) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            lastDebounceTime = currentMillis;
            executePurchase(buyOption);
        }
    }
    displayGold(220);
}

void Town::sellItems() {
    int sellOption = -1;
    renderSellMenu(sellOption);

    while (isAtShop && !isAtTown) {
        if (digitalRead(DOWN) == LOW) {  // Cycle through options
            while (digitalRead(DOWN) == LOW);  // Debounce
            sellOption = (sellOption + 1) % (player->inventory.itemCount + 1);  // Include exit option
            renderSellMenu(sellOption);
        }
        if (digitalRead(UP) == LOW) {  // Cycle through options
            while (digitalRead(UP) == LOW);  // Debounce
            sellOption = (sellOption -1);  // Include exit option
            if(sellOption <0){
              sellOption = (player->inventory.itemCount);
            }
            renderSellMenu(sellOption);
        }
        if (digitalRead(BACK) == LOW){
          while(digitalRead(BACK) == LOW);
          isAtShop = false;
          enterTown();
          return;
        }

        if (digitalRead(SELECT) == LOW) {  // Select option
            while (digitalRead(26) == LOW);  // Debounce
            if (sellOption == player->inventory.itemCount) {
                isAtShop = false;
                enterTown();
     
       } else {
                executeSale(sellOption);
            }
        }
    }
    displayGold(220);  // Update gold display after selling
}

void Town::renderBuyMenu(int buyOption) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (player->level < 7) {
      tft.drawString("1. Bamboo Spear - 10g", 10, 20, 2);
      tft.drawString("2. Club - 25g", 10, 40, 2);
      tft.drawString("3. Copper Sword - 100g", 10, 60, 2);
      tft.drawString("4. Leather Vest - 60g", 10, 80, 2);
      tft.drawString("5. Leather Armor - 100g", 10, 100, 2);
      tft.drawString("6. Herb - 10g", 10, 120, 2);
      tft.drawString("7. Exit", 10, 140, 2);
    } else {
        tft.drawString("1. Iron Sword - 300g", 10, 20, 2);
        tft.drawString("2. War-Axe - 350g", 10, 40, 2);
        tft.drawString("3. Steel Sword - 500g", 10, 60, 2);
        tft.drawString("4. Chainmail - 250g", 10, 80, 2);
        tft.drawString("5. Half-Plate - 500g", 10, 100, 2);
        tft.drawString("6. Full-Plate - 1000g", 10, 120, 2);
        tft.drawString("7. Exit", 10, 140, 2);
    }
        /*
        -equipment to add-
        Demon's Curse
        Demon's Bane
        Winged Blade
        Mithril Cloak
        Diamond Plate
        Angel's Grace
        -items to add-
        Minty Herb
        Dazzling Gem
        Tent
        Silver Feather
        Dragon's Soul
        */
        
    // Highlight selected option
    int yPosition = 20 + buyOption * 20;
    tft.drawRect(0, yPosition, 320, 20, TFT_RED);
    displayGold(220);
}

void Town::renderSellMenu(int sellOption) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    
    for (int i = 0; i < player->inventory.itemCount; i++) {
        tft.drawString(player->inventory.items[i]->name + " - Sell", 10, 20 + i * 20, 2);
    }
    tft.drawString("Exit", 10, 20 + player->inventory.itemCount * 20, 2);

    // Highlight selected option
    int yPosition = 20 + sellOption * 20;
    tft.drawRect(0, yPosition, 320, 20, TFT_RED);
    displayGold(220);
}

void Town::executePurchase(int buyOption) {
  unsigned long currentMillis = millis();
    const int numItems = 6; // Including the exit option
    lastDebounceTime = currentMillis;
    if (buyOption >= numItems) {
        isAtShop = false;
        enterTown();
        return;
    }
    if (player->level < 7) {
      int prices[] = {10, 25, 100, 60, 100, 10};
      String names[] = {"Bamboo Spear", "Club", "Copper Sword", "Leather Vest", "Leather Armor", "Herb"};
      String types[] = {"Weapon", "Weapon", "Weapon", "Armor", "Armor", "Consumable"}; // Example types
      int ATK[] = {5, 10, 20, 2, 4, 0}; // Example attack values
      int DEF[] = {0, 0, 0, 5, 10, 0}; // Example defense values
      int SPD[] = {0, 0, 0, 0, 0, 0}; // Example speed values

      if (buyOption < numItems && player->gold >= prices[buyOption]) {
        player->gold -= prices[buyOption];
        
        Item* purchasedItem = nullptr;
        if (types[buyOption] == "Consumable") {
            purchasedItem = new ConsumableItem(names[buyOption], 10, "heal", prices[buyOption]); // Potency and effect type are examples
        } else {
            purchasedItem = new Equipment(names[buyOption], ATK[buyOption], DEF[buyOption], SPD[buyOption], types[buyOption], prices[buyOption]);
          }
        if (purchasedItem) {
            player->inventory.addItem(purchasedItem);
            tft.fillRect(0, 200, 320, 40, TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawString("Purchased " + names[buyOption] + "!", 10, 200, 2);
        }
    } else {
        tft.fillRect(0, 200, 320, 40, TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("Not enough gold!", 10, 200, 2);
      }
    } else {
        int prices[] = {300, 350, 500, 250, 500, 1000};
        String names[] = {"Iron Sword", "War-Axe", "Steel Sword", "Chainmail", "Half-Plate", "Full-Plate"};
        String types[] = {"Weapon", "Weapon", "Weapon", "Armor", "Armor", "Armor"}; // Example types
        int ATK[] = {25, 35, 50, 2, 4, 0}; // Example attack values
        int DEF[] = {-5, 0, 0, 15, 25, 40}; // Example defense values
        int SPD[] = {0, -5, 5, 3, 0,-5}; // Example speed values

        if (buyOption < numItems && player->gold >= prices[buyOption]) {
        player->gold -= prices[buyOption];
        
        Item* purchasedItem = nullptr;
        if (types[buyOption] == "Consumable") {
            purchasedItem = new ConsumableItem(names[buyOption], 10, "heal", prices[buyOption]); // Potency and effect type are examples
        } else {
            purchasedItem = new Equipment(names[buyOption], ATK[buyOption], DEF[buyOption], SPD[buyOption], types[buyOption], prices[buyOption]);
          }
        if (purchasedItem) {
            player->inventory.addItem(purchasedItem);
            tft.fillRect(0, 200, 320, 40, TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawString("Purchased " + names[buyOption] + "!", 10, 200, 2);
        }
        } else {
            tft.fillRect(0, 200, 320, 40, TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawString("Not enough gold!", 10, 200, 2);
          }
    }
    delay(2000);
    renderBuyMenu(buyOption);
}

void Town::executeSale(int sellOption) {
    if (sellOption == player->inventory.itemCount) {
        isAtShop = false;
        enterTown();
        return;
    }

    if (sellOption >= 0 && sellOption < player->inventory.itemCount) {
        Item* item = player->inventory.items[sellOption];
        Item* itemToSell = player->inventory.items[sellOption];
        //create a variable called 'equipment' which is a pointer to the equipment class, and then give it a value which is determined
        // by the sellOption variable, which we achieve through a static cast of the above Item pointer
        Equipment* equipment = static_cast<Equipment*>(item);
        tft.fillRect(0, 200, 320, 40, TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        // Check if the item is equipped
    if (item->isEquipment() && equipment->isEquipped) {
        player->inventory.useItem(sellOption, *player);  // Unequip the item before selling
    }
        tft.drawString("Sold " + item->name + "!", 10, 200, 2);
        player->gold += item->getSellValue();
        player->inventory.removeItem(sellOption); // Assumes removeItem handles deletion
        delay(2000);
    }
    renderSellMenu(sellOption);
}


void Town::renderShopMenu(int shopOption) {
    tft.fillScreen(TFT_BLACK);
    String options[] = {"Buy", "Sell", "Exit"};
    for (int i = 0; i < 3; i++) {
        if (i == shopOption) {
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(options[i], 10 + i * 100, 20, 2); // Adjust X position as needed
    }
    displayGold(220);
}

void Town::renderTownMenu(int townOption) {
    int menuY = 200; // Example Y position for menu
    tft.fillRect(0, menuY, 320, 40, TFT_BLACK);
    String options[] = {"Shop", "Inn", "Explore"};
    for (int i = 0; i < 3; i++) {
        if (i == townOption) { // Assume townOption is the current selected option
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(options[i], 10 + i * 100, menuY, 2); // Adjust X position as needed
    }
    displayGold(220);
    tft.setTextSize(1);
    tft.setTextColor(TFT_BLUE);
    tft.setCursor(45, 180);
    tft.print(player->name); tft.setTextColor(TFT_WHITE); tft.print(" ATK: "); tft.print(player->ATK); tft.print(" DEF: "); tft.print(player->DEF); tft.print(" SPD: "); tft.println(player->SPD);
}

void Town::stayAtInn() {
    isAtTown = false;
    int innOption = 0;
    renderInnMenu(innOption);
    while (isAtInn) {
        unsigned long currentMillis = millis();
        if (digitalRead(DOWN) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            lastDebounceTime = currentMillis;
            innOption = (innOption + 1) % 2;
            renderInnMenu(innOption);
        } else if (digitalRead(UP) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            lastDebounceTime = currentMillis;
            innOption = (innOption - 1) % 2;
            renderInnMenu(innOption);
        }
        if (digitalRead(BACK) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
          lastDebounceTime = currentMillis;
          isAtInn = false;
          enterTown();
        }

        if (digitalRead(SELECT) == LOW && currentMillis - lastDebounceTime > debounceDelay) {
            lastDebounceTime = currentMillis;
            switch (innOption) {
                case 0:
                    restAtInn();
                    break;
                case 1:
                    isAtInn = false;
                    enterTown();
                    break;
            }
        }
    }
}

void Town::renderInnMenu(int innOption) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    String options[] = {"Rest", "Exit"};
    for (int i = 0; i < 2; i++) {
        if (i == innOption) {
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(options[i], 10, 20 + i * 20, 2);
    }
    displayGold(220);
}

void Town::restAtInn() {
    const int costPerLevel = 2; // Cost per level to rest
    int restCost = player->level * costPerLevel;
    if (player->gold >= restCost) {
        player->HP = player->getMaxHp();
        player->MP = player->getMaxMp();
        player->gold -= restCost;

        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("Rested well!", 10, 20, 2);
        tft.drawString("Gold: -" + String(restCost), 10, 40, 2);
    } else {
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("Not enough gold to rest!", 10, 20, 2);
    }
    delay(2000); // Show the message for a moment

    isAtInn = false; // Exit the inn after the action
    enterTown(); // Return to the town menu
}

void Town::displayGold(int yPosition) {
    // Only update the display if the value has changed
        // Clear the area for the status only if values have changed
        tft.fillRect(0, yPosition, 320, 20, TFT_BLACK);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("Gold: " + String(player->gold), 45, yPosition, 2);
        tft.setTextColor(TFT_WHITE);
}

void Town::explore() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
  tft.print("Exiting Town...");
  delay(800);
  tft.fillScreen(TFT_BLACK);
    // Set player position to just outside the town's exit
    // Initialize or reset any necessary exploration state variables here
    exploreSystem->beginExplore = true;  // Indicate that exploration should begin
    isExploring = true;
    isAtTown = false;  // Ensure we're no longer in the town state
}

void Town::handleTownInput() {
  int townOption = 0;
    renderTownMenu(townOption);
    while (isAtTown) {
        unsigned long currentMillis = millis();
        if (digitalRead(LEFT) == LOW) {
            if (currentMillis - lastDebounceTime > debounceDelay) {
                townOption--;
                if(townOption < 0) {
                  townOption = 2;
                }
                renderTownMenu(townOption);
                lastDebounceTime = currentMillis;
            }
        } else if (digitalRead(RIGHT) == LOW) {
            if (currentMillis - lastDebounceTime > debounceDelay) {
                townOption++; 
                if (townOption > 2) {
                  townOption = 0;
                }
                renderTownMenu(townOption);
                lastDebounceTime = currentMillis;
            }
        }

        if (digitalRead(SELECT) == LOW) {
            if (currentMillis - lastDebounceTime > debounceDelay) {
                switch (townOption) {
                    case 0:
                        isAtShop = true;
                        isAtTown = false;
                        visitShop();
                        delay(300);
                        break;
                    case 1:
                        isAtInn = true;
                        isAtTown = false;
                        stayAtInn();
                        delay(300);
                        break;
                    case 2:
                        explore();
                        break;
                }
                lastDebounceTime = currentMillis;
                townOption = 0;
            }
        }
    }
}

