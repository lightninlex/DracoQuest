//Explore.cpp

#include "Explore.h"
#include "Creature.h"
#include "Common.h"
#include <TFT_eSPI.h>  // For the display

const int UP = 32;
const int DOWN = 33;
const int LEFT = 25;
const int RIGHT = 26;
const int BACK = 27;
const int SELECT = 14;
unsigned long lastDebounce = 0;
const int debounceDelay = 150;

Explore::Explore(Creature* player) : player(player) {
    // Set town's zone coordinates
    townZoneX = 1;  // Corresponds to zone 6 in your grid
    townZoneY = 1;

    // Set the specific area within the zone for the town
    int townAreaX = 1; // For area 2 in the zone
    int townAreaY = 0; // Top row
    // Start exploring in zone 6, area 1
    zoneX = townZoneX;
    zoneY = townZoneY;

    // Calculate player's start position in the grid, assuming area 1 is top-left in the zone
    playerX = zoneX * AREA_WIDTH + 0; // Leftmost column in the zone
    playerY = zoneY * AREA_HEIGHT + 0; // Top row in the zone
    
    isExploring = false;  // Should be false initially, set to true when exploration starts
    enteringTown = false;
    discoveryFound = false;
    inOverworld = false;
    inDiscovery = false;
    inCombat = false;
    updateCurrentZoneAndArea();
}


void Explore::handleInput() {
  unsigned long currentMillis = millis();
    // Check for button presses to navigate the menu or make selections
    if (digitalRead(LEFT) == LOW && currentMillis - lastDebounce > debounceDelay) {  // Assuming 25 is the button for navigating
      lastDebounce = currentMillis;
        exploreOption = (exploreOption - 1) % 5;  // Assuming 6 options including item and exit
        if (exploreOption < 0) {
          exploreOption = 5;
        }
        displayExploreMenu();
    } else if (digitalRead(RIGHT) == LOW && currentMillis - lastDebounce > debounceDelay) {
      lastDebounce = currentMillis;
        exploreOption = (exploreOption + 1) % 5;  // Assuming 6 options including item and exit
        if (exploreOption > 5) {
          exploreOption = 0;
        }
        displayExploreMenu();
    }

    if (digitalRead(SELECT) == LOW && currentMillis - lastDebounce > debounceDelay) {  // Assuming 26 is the button for selection
        lastDebounce = currentMillis;

        // Execute the selected option's action
        performExploreAction();

        // Return to the main exploration menu after performing the action
        if (!inItemMenu) {
            displayExploreMenu();
        }
    }
}

void Explore::startExplore() {
  updateCurrentZoneAndArea();
  tft.fillRect(0, menuStartY, screenWidth, menuHeight, TFT_BLACK);
    String menuItems[] = {"NORTH", "SOUTH", "EAST", "WEST", "ITEM"};
    int menuItemX[] = {5, 50, 95, 130, 170};
    for (int i = 0; i < 5; i++) {
      if (i==exploreOption) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
      } else {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
      }
      tft.drawString(menuItems[i], menuItemX[i], menuStartY, 2);
    }
    exploreOption = 0;
}
void Explore::displayExploreMenu() {
  messageY = 0;
  
  // Display the current zone and area in the message window
  String zoneAreaMessage = "Zone: " + String(currentZone) + " Area: " + String(currentArea) + "Type:" + String(zoneType);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(zoneAreaMessage, 10, messageY, 2);
  if (exploreOption != prevExploreOption) {
    tft.fillRect(0, menuStartY, screenWidth, menuHeight, TFT_BLACK);
    String menuItems[] = {"NORTH", "SOUTH", "EAST", "WEST", "ITEM"};
    int menuItemX[] = {5, 50, 95, 130, 170};
    for (int i = 0; i < 5; i++) {
      if (i==exploreOption) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
      } else {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
      }
      tft.drawString(menuItems[i], menuItemX[i], menuStartY, 2);
    }
  } else if (exploreOption== -1 && prevExploreOption == 0) {
    exploreOption = 0;
    tft.fillRect(0, menuStartY, screenWidth, menuHeight, TFT_BLACK);
    String menuItems[] = {"NORTH", "SOUTH", "EAST", "WEST", "ITEM"};
    int menuItemX[] = {5, 50, 95, 130, 170};
    for (int i = 0; i < 5; i++) {
      if (i==exploreOption) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
      } else {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
      }
      tft.drawString(menuItems[i], menuItemX[i], menuStartY, 2);
    }
  }
      prevExploreOption = exploreOption;
}

void Explore::performExploreAction() {
    tft.fillScreen(TFT_BLACK);
    displayExploreMenu();
    int messageY = 0;
    tft.fillRect(0, messageY, 320, 40, TFT_BLACK);  // Clear the message area
        // Execute movement or item use based on exploreOption
        switch (exploreOption) {
            case 0: // North
                moveNorth();
                break;
            case 1: // South
                moveSouth();
                break;
            case 2: // East
                moveEast();
                break;
            case 3: // West
                moveWest();
                break;
            case 4: // Item
                useItem();
                break;
        }
        
        prevExploreOption = exploreOption;
        exploreOption = -1;
        displayExploreMenu();
        delay(800);  // Wait a bit before clearing the message
    }


void Explore::resolveExplore() {
    if (inCombat) {
    exploreOption = prevExploreOption;
    prevExploreOption = -1;
        beginCombat = true;  // This flag should trigger combat in the game update loop
        inCombat = false;
    } else if (foundTreasure) {
        // Handle treasure discovery, e.g., add items or currency to the player's inventory
        tft.drawString("You've discovered some loot along the path!", 10, 30, 2);
        delay(800);
        getTreasure();
        foundTreasure = false;  // Reset the flag after handling
    }
    delay(400);
    tft.fillRect(0, 0, 320, 80, TFT_BLACK);
    exploreOption = prevExploreOption;
    prevExploreOption = -1;
    // Reset discovery flags after resolving
    discoveryFound = false;
    inDiscovery = false;
}

void Explore::getTreasure() {
  // modify to include new zone types and time of day
  tft.fillScreen(TFT_BLACK);
    int treasureType = random(1, 4);  // Randomly decide the type of treasure
    int messageY = 30;  // Y position to start displaying messages on the TFT

    switch (treasureType) {
        case 1: { // Gold
            tft.setTextColor(TFT_YELLOW);
            int threshold = random(1, 101); // Determine the rarity threshold
            int goldAmount = 0;
            if (threshold <= 60) {  // Low threshold
                goldAmount = random(20, 41);
            } else if (threshold <= 85) {  // Medium threshold
                goldAmount = random(40, 61);
            } else {  // High threshold
                goldAmount = random(60, 101);
            }
            player->gold += goldAmount;
            tft.drawString("Found gold: " + String(goldAmount), 10, messageY, 2);
            break;
        }
        case 2: { // Herb
            tft.setTextColor(TFT_GREEN);
            int herbCount = random(1, 6);  // Random amount between 1 and 5
            for (int i = 0; i < herbCount; i++) {
                ConsumableItem* herb = new ConsumableItem("Herb", 10, "heal", 5);
                player->inventory.addItem(herb);
            }
            tft.drawString("Found herbs: " + String(herbCount), 10, messageY, 2);
            break;
        }
        case 3: { // Potion
            tft.setTextColor(TFT_MAGENTA);
            int potionCount = random(1, 3);  // 1 or 2 potions
            for (int i = 0; i < potionCount; i++) {
                ConsumableItem* potion = new ConsumableItem("Potion", 20, "mana", 10);
                player->inventory.addItem(potion);
            }
            tft.drawString("Found potions: " + String(potionCount), 10, messageY, 2);
            break;
        }
    }
    tft.setTextColor(TFT_WHITE);
    delay(800);
}


void Explore::updateCurrentZoneAndArea() {
    // Calculate the player's current zone based on their position
    zoneX = playerX / AREA_WIDTH;
    zoneY = playerY / AREA_HEIGHT;

    // Calculate the player's current area within the zone
    areaX = playerX % AREA_WIDTH;
    areaY = playerY % AREA_HEIGHT;

    // Update the current zone and area IDs
    currentZone = zones[zoneY][zoneX];
    currentArea = areas[areaY][areaX];

    // Determine the type of the current zone
    zoneType = zoneTypes[zoneY][zoneX];
    enteringTown = (zoneX == townZoneX && zoneY == townZoneY && areaX == townAreaX && areaY == townAreaY);

    // Check if the player has entered the town's area
    // The town is in zone 6, area 2 (assuming areas start from 1)
    // Since townAreaX and townAreaY are 0-indexed, they should be set to 0 and 1 respectively
    displayExploreMenu();
    
}

void Explore::exploreUpdate() {
    if (isExploring) {
        displayExploreMenu();
        handleInput();
    }
    if(enteringTown){
      !isExploring;
    }
}

void Explore::chanceDiscovery() {
  // modify to include new zone types and time of day
    // Random logic to determine if something is discovered
    int chance = random(0, 100);
    discoveryFound = false;

    // Common event: Combat
    if (chance < 60) {  // Common discovery: Combat
        inDiscovery = true;
        discoveryFound = true;
        inCombat = true;
    }
    // Uncommon event: Nothing
    else if (chance >= 60 && chance < 89) {
        inDiscovery = true;
        discoveryFound = true;
    }
    // Rare event: Treasure
    else if (chance >= 90 && chance < 100) {
        inDiscovery = true;
        discoveryFound = true;
        foundTreasure = true;
    }
    if (discoveryFound) {
        resolveExplore();
    }
}

void Explore::getArea() {
    areaX = playerX % AREA_WIDTH;
    areaY = playerY % AREA_HEIGHT;
    currentArea = areaY * AREA_WIDTH + areaX + 1;
}

void Explore::getZone() {
    zoneX = playerX / AREA_WIDTH;
    zoneY = playerY / AREA_HEIGHT;
    currentZone = zones[zoneY][zoneX];
    zoneType = zoneTypes[zoneY][zoneX];
}

void Explore::moveNorth() {
    if (playerY > 0) {
        playerY--;
    } else if (zoneY > 0) {
        zoneY--;
        playerY = zoneY * AREA_HEIGHT + (AREA_HEIGHT - 1);  // Move to the bottom area of the northern zone
    }
    updateCurrentZoneAndArea();
    if(!enteringTown){
      chanceDiscovery();
    }
    
}

void Explore::moveSouth() {
    if (playerY < (ZONE_HEIGHT * AREA_HEIGHT - 1)) {
        playerY++;
        if (areaY < AREA_HEIGHT - 1) {
            areaY++;
        } else {
            areaY = 0;
            zoneY++;
        }
    }
    updateCurrentZoneAndArea();
    if(!enteringTown){
      chanceDiscovery();
    }
}

void Explore::moveEast() {
    if (playerX < (ZONE_WIDTH * AREA_WIDTH - 1)) {
        playerX++;
        if (areaX < AREA_WIDTH - 1) {
            areaX++;
        } else {
            areaX = 0;
            zoneX++;
        }
    }
    updateCurrentZoneAndArea();
    if(!enteringTown){
      chanceDiscovery();
    }
}

void Explore::moveWest() {
    if (playerX > 0) {
        playerX--;
        if (areaX > 0) {
            areaX--;
        } else {
            areaX = AREA_WIDTH - 1;
            zoneX--;
        }
    }
    updateCurrentZoneAndArea();
    if(!enteringTown){
      chanceDiscovery();
    }
}

void Explore::useItem() {
    tft.fillScreen(TFT_BLACK);
    delay(600);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    int itemIndex = 0;  // Initialize to 0 to start at the first item
    bool itemSelected = false;

    if (player->inventory.itemCount == 0) {
        tft.drawString("No items in inventory", 10, 20, 2);
        delay(2000);  // Show message for a short time
        tft.fillScreen(TFT_BLACK);
        displayExploreMenu();
        return;  // Exit the function if the inventory is empty
    }

    while (!itemSelected) {
        // Display the inventory items and 'Exit' option
        for (int i = 0; i < player->inventory.itemCount; i++) {
            Item* item = player->inventory.items[i];
            String itemName = player->inventory.items[i]->name;
            if (item->isEquipment()) {
            Equipment* equipment = static_cast<Equipment*>(item);
            itemName += equipment->isEquipped ? " (E)" : " (Equip)";
        } else if (item->isConsumable()) {
            itemName += " (Use)";
        }
            tft.drawString(itemName, 10, 20 + i * 20, 2);
        }
        tft.drawString("Exit", 10, 20 + player->inventory.itemCount * 20, 2);

        // Highlight the selected option
        int yPosition = 20 + itemIndex * 20;
        tft.drawRect(0, yPosition, 320, 20, TFT_RED);

        if (digitalRead(BACK) == LOW) {
            while (digitalRead(BACK) == LOW);  // Wait for button release
            itemSelected = true;
            tft.fillScreen(TFT_BLACK);
            continue;  // Skip the rest of the loop and exit
        }

        if (digitalRead(UP) == LOW) {
            while (digitalRead(UP) == LOW);  // Wait for button release
            if (itemIndex > 0) {
                itemIndex--;
                tft.fillScreen(TFT_BLACK);
            } else {
                itemIndex = player->inventory.itemCount;  // Go to 'Exit' option
                tft.fillScreen(TFT_BLACK);
            }
        } else if (digitalRead(DOWN) == LOW) {
            while (digitalRead(DOWN) == LOW);  // Wait for button release
            itemIndex = (itemIndex + 1) % (player->inventory.itemCount + 1);
            tft.fillScreen(TFT_BLACK);
        } else if (digitalRead(SELECT) == LOW) {
            while (digitalRead(SELECT) == LOW);  // Wait for button release
            if (itemIndex == player->inventory.itemCount) {
                itemSelected = true;  // Exit option selected
            } else {
                // An item is selected
                Item* item = player->inventory.items[itemIndex];
                if (item->isConsumable()) {
                    player->inventory.useItem(itemIndex, *player);
                    itemSelected = true;

                } else if (item->isEquipment()) {
                    player->inventory.useItem(itemIndex, *player);
                }
            }
            delay(500);
            tft.fillScreen(TFT_BLACK);
        }
    }
}
