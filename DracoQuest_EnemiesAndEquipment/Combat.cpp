#include "Combat.h"
#include "Common.h"
#include "Equipment.h"
#include "ConsumableItem.h"
#include <TFT_eSPI.h>

const int UP = 32;
const int DOWN = 33;
const int LEFT = 25;
const int RIGHT = 26;
const int BACK = 27;
const int SELECT = 14;

Combat::Combat(Creature* player, EnemyManager* enemyManager)
: player(player), enemyManager(enemyManager)  {
    this->enemy = nullptr;
    this->combatOption = 0; // Default combat option
    this->prevCombatOption = -1;
    this->inSpellMenu = false; // Not in spell menu at start
    this->spellOption = 0; // Default spell option
    this->inItemMenu = false;
}

void Combat::startCombat() {

  combatOver = false;
    inCombat = true;  
    tft.fillScreen(TFT_BLACK);
    String menuItems[] = {"Attack", "Spell", "Item", "Flee"};
    int menuItemX[] = {10, 80, 150, 220};
    // Display each menu item
        for (int i = 0; i < 4; i++) {  // Now looping through 4 options
        if (i == combatOption) {
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(menuItems[i], menuItemX[i], menuStartY, 2);
    }
    renderCombatMenu();  
    // Define the Y position for the message, consistent with resolveCombat()
    int messageY = 0;

    // Clear the area where the message will be displayed
    tft.fillRect(0, messageY, 320, 40, TFT_BLACK);

    // Set text color for the enemy appearance message
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // Construct and display the message
    String message = "A " + enemy->name + " appears before you!";
    tft.drawString(message, 10, messageY, 1); 
    delay(800);
}

void Combat::handleInput() {
    unsigned long currentTime = millis();
    if (currentTime - lastInputTime > debounceDelay) {
        if (digitalRead(RIGHT) == LOW) {
            while (digitalRead(RIGHT) == LOW);
            lastInputTime = millis();

            if (inSpellMenu || inItemMenu) {
                // No action on RIGHT button for spell or item menu
            } else {
                combatOption = (combatOption + 1) % 4;
                renderCombatMenu();
            }
        }

        if (digitalRead(LEFT) == LOW) {
            while (digitalRead(LEFT) == LOW);
            lastInputTime = millis();

            if (inSpellMenu || inItemMenu) {
                // No action on LEFT button for spell or item menu
            } else {
                combatOption = (combatOption - 1 + 4) % 4;
                renderCombatMenu();
            }
        }

        if (digitalRead(DOWN) == LOW) {
            while (digitalRead(DOWN) == LOW);
            lastInputTime = millis();

            if (inSpellMenu) {
                spellOption = (spellOption + 1) % numSpellOptions;
                displaySpellMenu();
            } else if (inItemMenu) {
                itemOption = (itemOption + 1) % player->inventory.itemCount;
                displayItemMenu();
            }
        }

        if (digitalRead(UP) == LOW) {
            while (digitalRead(UP) == LOW);
            lastInputTime = millis();

            if (inSpellMenu) {
                spellOption = (spellOption - 1 + numSpellOptions) % numSpellOptions;
                displaySpellMenu();
            } else if (inItemMenu) {
                itemOption = (itemOption - 1 + player->inventory.itemCount) % player->inventory.itemCount;
                displayItemMenu();
            }
        }

        if (digitalRead(BACK) == LOW) {
            while (digitalRead(BACK) == LOW);
            lastInputTime = millis();
            if (inSpellMenu) {
                inSpellMenu = false;
                prevCombatOption = combatOption;
                combatOption = 0;
                renderCombatMenu();
            }
            if (inItemMenu) {
                inItemMenu = false;
                prevCombatOption = combatOption;
                combatOption = 0;
                renderCombatMenu();
            }
        }

        if (digitalRead(SELECT) == LOW) {
            while (digitalRead(SELECT) == LOW);
            lastInputTime = millis();

            if (inSpellMenu) {
                if (spellOption == numSpellOptions - 1) {
                    inSpellMenu = false;
                } else {
                    performPlayerAction();
                }
                renderCombatMenu();
            } else if (inItemMenu) {
                if (itemOption == player->inventory.itemCount) {
                    inItemMenu = false;
                } else {
                    performPlayerAction();
                }
                renderCombatMenu();
            } else {
                performPlayerAction();
            }
        }
    }
    tft.setTextColor(TFT_WHITE);
}

void Combat::performPlayerAction() {
    if (!enemy || player->HP <= 0 || enemy->HP <= 0) {
        resolveCombat();
        return;
    }

    int messageY = 0;
    tft.fillRect(0, messageY, 320, 100, TFT_BLACK); // Clear any previous messages

      if (inItemMenu && itemOption < player->inventory.itemCount) {
        // Use the selected item
        Item* item = player->inventory.items[itemOption];
        if (item->isConsumable()) {
            ConsumableItem* consumable = static_cast<ConsumableItem*>(item);
            player->inventory.useItem(itemOption, *player);
            inItemMenu = false;
        } else if(item->isEquipment()) {
            Equipment* equipment = static_cast<Equipment*>(item);
            player->inventory.useItem(itemOption, *player);
            inItemMenu = false;
        }
        playerActionComplete = true;
        renderCombatMenu();
    }
    if (inSpellMenu) {
        String spells[] = {"Heal", "Hurt", "Fireball", "Blessing", "Ice Storm", "Thunder"};
        int spellLevels[] = {3, 4, 5, 8, 10, 12};
        int numSpells = sizeof(spells) / sizeof(spells[0]);
        
        // Check if Back option is selected
        if (spellOption == numSpells) {
            inSpellMenu = false;
            prevCombatOption = combatOption;
            combatOption = 0;
            renderCombatMenu();
            return;
        }

        String selectedSpell = spells[spellOption];
        if (selectedSpell == "Heal" && player->canCastSpell("Heal") && player->MP >= 3) {
            int healAmount = random(10, 26);
            int actualHeal = min(healAmount, player->getMaxHp() - player->HP);
            player->HP += actualHeal;
            player->MP -= 3;
            tft.drawString("With a divine burst, " + player->name + " casts Heal for " + String(actualHeal) + " HP!", 10, messageY, 1);
            playerActionComplete = true; // Prevent enemy action until the next loop iteration
        } else if (selectedSpell == "Hurt" && player->canCastSpell("Hurt") && player->MP >= 4) {
            int damage = random(15, 26);
            enemy->HP -= damage;
            player->MP -= 4;
            tft.drawString("A shadow falls upon the " + enemy->name + ",", 10, messageY, 1); 
            messageY+=20; tft.drawString("as you cast Hurt, dealing " + String(damage) + " damage!", 10, messageY, 1);
            playerActionComplete = true; // Prevent enemy action until the next loop iteration
        } else if (selectedSpell == "Fireball" && player->canCastSpell("Fireball") && player->MP >= 6) {
            int damage = random(30, 46);
            enemy->HP -= damage;
            player->MP -= 6;
            tft.drawString("A blistering inferno bursts from your hand.", 10, messageY, 1); 
            messageY+=20; 
            tft.drawString("A Fireball is cast, dealing " + String(damage) + " damage!", 10, messageY, 1);
            playerActionComplete = true; // Prevent enemy action until the next loop iteration
        } else if (selectedSpell == "Blessing" && player->canCastSpell("Blessing") && player->MP >= 8) {
            int healAmount = random(20, 41);
            player->HP += healAmount;
            player->MP -= 8;
            tft.drawString("A calming light envelops you, as Blessing is cast, for " + String(healAmount) + " HP!", 10, messageY, 1);
            playerActionComplete = true; // Prevent enemy action until the next loop iteration
        } else if (selectedSpell == "Ice Storm" && player->canCastSpell("Ice Storm") && player->MP >= 10) {
            int damage = random(45, 56);
            enemy->HP -= damage;
            player->MP -= 10;
            tft.drawString("A blight frost chills the air. You summon an Ice Storm,", 10, messageY, 1); 
            messageY+=20; 
            tft.drawString("dealing " + String(damage) + " damage!", 10, messageY, 1);
            playerActionComplete = true; // Prevent enemy action until the next loop iteration
        } else if (selectedSpell == "Thunder" && player->canCastSpell("Thunder") && player->MP >= 12) {
            int damage = random(55, 66);
            enemy->HP -= damage;
            player->MP -= 12;
            tft.drawString("The skies darken and roar, as you call down", 10, messageY, 1); 
            messageY+=20; 
            tft.drawString("Thunder, dealing " + String(damage) + " damage!", 10, messageY, 1);
            playerActionComplete = true; // Prevent enemy action until the "next" loop iteration
        } else {
            tft.drawString("Not enough MP or level to cast " + selectedSpell + "!", 10, messageY, 1);
        }

        inSpellMenu = false; // Exit the spell menu after an action
        renderCombatMenu(); // Refresh the combat menu
    } else {
        switch (combatOption) {
            case 0: // Attack
                player->attack(*enemy);
                if (player->criticalHit == true) {
                tft.drawString("With a fierce roar, you pulverize", 10, messageY, 1); 
                messageY += 20;
                tft.drawString("the " + enemy->name + "!", 10, messageY, 1);
                messageY += 20;
                tft.setCursor(10, messageY);
                tft.print("A critical hit!");
                delay(700);
                player->criticalHit = false;
                } else {
                tft.drawString("You strike at the " + enemy->name + "!", 10, messageY, 1);
                }
                break;
            case 1: // Spell
                inSpellMenu = true;
                spellOption = 0;
                displaySpellMenu();
                break;
            case 2:  // Item
                inItemMenu = true;
                itemOption = 0;
                displayItemMenu();
                break;
            case 3: // Flee
                if (player->tryToFlee(*enemy)) {
                    tft.setTextColor(TFT_WHITE);
                    tft.drawString("You escaped safely!", 10, messageY, 1);
                    delay(800);
                    tft.fillScreen(TFT_BLACK);
                    combatOver = true;
                    inCombat = false;
                    return;
                } else {
                    tft.setTextColor(TFT_WHITE);
                    tft.drawString("You failed to flee!", 10, messageY, 1);
                    
                    renderCombatMenu();
                }
                break;
        }
    }
    delay(800); // Show the action message for a brief period
    if(!inSpellMenu && !inItemMenu) {
      playerActionComplete = true; // Prevent enemy action until the next loop iteration
      prevCombatOption = combatOption;
      combatOption = 0;
    }
}

void Combat::combatTurn() {

    if (player->HP <= 0 || enemy->HP <= 0) {
        resolveCombat();
        return;
    }
    int messageY = 0; // Position for combat resolution messages at the top
    tft.fillRect(0, messageY, 320, 100, TFT_BLACK); // Clear the area for messages
    // Enemy's turn
    if (enemy->shouldEnemyFlee(*player)) {
        int messageY = 20;  // Define Y position for messages
        String message = "The " + enemy->name + " seems fearful, and tries to flee!";
        tft.fillRect(0, messageY, 320, 20, TFT_BLACK); // Clear the previous message
        tft.drawString(message, 10, messageY, 1);
        delay(800);
        if (random(100) < enemy->SPD - player->SPD) {  // Flee success check
            inCombat = false;
            message = "The " + enemy->name + " successfully fled!";
            tft.fillRect(0, messageY, 320, 20, TFT_BLACK); // Clear the "tries to flee" message
            tft.drawString(message, 10, messageY, 1); // Display flee success message at the same position
            delay(800);
            combatOver = true;
            inCombat = false;
        } else {
            message = "The " + enemy->name + " couldn't escape you!";
            tft.fillRect(0, messageY, 320, 20, TFT_BLACK); // Clear the "tries to flee" message
            tft.drawString(message, 10, messageY, 1); // Display flee failure message
            delay(800);
        }
    } else {
        if (enemy->criticalHit == true) {
            enemy->combatAction(*player);
            int messageY = 0;
            String actionMessage = "The " + enemy->name + " tears into you with a fierce attack!";
            tft.fillRect(0, messageY, 320, 100, TFT_BLACK); // Clear any previous message
            tft.drawString(actionMessage, 10, messageY, 1);
            messageY += 20;
            tft.setCursor(10, messageY);
            tft.print("A critical strike!");
            delay(800);
            enemy->criticalHit = false;
        } else {
            enemy->combatAction(*player);
            int messageY = 20;
            if (enemy->name == "Dracky" && enemy->MP >=4 && enemy->HP < enemy->getMaxHp() * 0.5) {
            String actionMessage = "The " + enemy->name + " cast Hurt!";
            tft.fillRect(0, messageY, 320, 20, TFT_BLACK); // Clear any previous message
            tft.drawString(actionMessage, 10, messageY, 1);
            } else if (enemy->name == "Ghost" && enemy->MP >= 6 && enemy->level > 8 && enemy->HP < enemy->getMaxHp() * .5) {
              String actionMessage = "The " + enemy->name + " cast Hurt!";
            tft.fillRect(0, messageY, 320, 20, TFT_BLACK); // Clear any previous message
            tft.drawString(actionMessage, 10, messageY, 1);
            } else if (enemy->name == "SkeletonKnight" && enemy->MP >= 3 && enemy->HP < enemy->getMaxHp() * .5) {
              String actionMessage = "The " + enemy->name + " cast Heal!";
            tft.fillRect(0, messageY, 320, 20, TFT_BLACK); // Clear any previous message
            tft.drawString(actionMessage, 10, messageY, 1);
            } else if (enemy->name == "SkeletonKnight" && enemy->MP >= 6 && enemy->HP < enemy->getMaxHp() * .3) {
              String actionMessage = "The " + enemy->name + " cast Fireball!";
            tft.fillRect(0, messageY, 320, 20, TFT_BLACK); // Clear any previous message
            tft.drawString(actionMessage, 10, messageY, 1);
            } else if (enemy->name == "Necromancer" && enemy->MP >= 8 && enemy->HP < enemy->getMaxHp() * .4) {
              String actionMessage = "The " + enemy->name + " cast Unholy Blessing";
            tft.fillRect(0, messageY, 320, 20, TFT_BLACK); // Clear any previous message
            tft.drawString(actionMessage, 10, messageY, 1);
            } else {
            messageY = 0;
            String actionMessage = "The " + enemy->name + " strikes you!";
            tft.fillRect(0, messageY, 320, 100, TFT_BLACK); // Clear any previous message
            tft.drawString(actionMessage, 10, messageY, 1);
            }
        }
    }
    delay(1000);
    if (player->HP <= 0 || enemy->HP <= 0) {
        resolveCombat();
        return;
    }
    renderCombatMenu();
}

void Combat::resolveCombat() {
    int messageY = 0; // Position for combat resolution messages at the top
    int additionalMessageY = 20; // For additional messages below the main one
    tft.fillRect(0, messageY, 320, 100, TFT_BLACK); // Clear the area for messages

    if (enemy->HP <= 0) {
        // Display victory message on TFT at the top
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.drawString("Enemy defeated!", 10, messageY, 1);
          delay(800);
        // Display gold and XP earned below the victory message
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("You have gained " + String(enemy->gold) + " gold, and " + String(enemy->xp) + " experience!", 10, additionalMessageY, 1);
    } else if (player->HP <= 0) {
        // Display defeat message on TFT at the top
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawString("You were defeated...", 10, messageY, 1);
        delay(800);
        
    }
    // Update player stats and reset as necessary after acknowledgment
    if (enemy->HP <= 0) {
        player->gold += enemy->gold; // Award gold
        player->xp += enemy->xp; // Award XP
        player->checkLevelUp(); // Check if the player leveled up
        combatOver = true;
    } else if (player->HP <= 0) {
        player->HP = player->getMaxHp();
        player->MP = player->getMaxMp();
        player->gold -= player->gold/2;
        isDead = true;
    }

    delay(800);
    tft.fillScreen(TFT_BLACK);
    combatOver = true;
    inCombat = false;
    
}

void Combat::renderCombatMenu() {
    int screenWidth = 320;
    int screenHeight = 240;
    int menuHeight = 60;
    int statusHeight = 20;
    int menuStartY = screenHeight - menuHeight;
    int statusStartY = menuStartY - statusHeight;

    // Display combat status above the menu
    displayCombatStatus(statusStartY);

    // Only redraw menu options if the selected option has changed
    if (combatOption != prevCombatOption && inSpellMenu == false && inItemMenu == false) {
        // Clear the area for the menu only if the option has changed
        tft.fillRect(0, menuStartY, screenWidth, menuHeight, TFT_BLACK);

        // Define menu items and their positions
        String menuItems[] = {"Attack", "Spell", "Item", "Flee"};
         int menuItemX[] = {10, 80, 150, 220};  // Adjust positions to fit

        // Display each menu item
        for (int i = 0; i < 4; i++) {  // Now looping through 4 options
        if (i == combatOption) {
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.drawString(menuItems[i], menuItemX[i], menuStartY, 2);
    }
  }

        // Update the previous combat option
        prevCombatOption = combatOption;
        if (combatOption < 0) {
          combatOption = 3;
        }
        if (combatOption > 3) {
          combatOption = 0;
        }
}

void Combat::displayItemMenu() {
    int menuStartY = 180;
    tft.fillRect(0, menuStartY, 320, 60, TFT_BLACK);

    int itemCount = player->inventory.itemCount;
    int maxDisplayItems = min(MAX_ITEMS_DISPLAY, itemCount); // Number of items to display on screen

    // Ensure itemOption is within valid range
    itemOption = max(0, min(itemOption, itemCount));

    // Calculate the starting index for the visible items
    int startIdx = max(0, itemOption - maxDisplayItems + 1);

    // Display items with scrolling
    for (int i = 0; i < maxDisplayItems && startIdx + i < itemCount; i++) {
        Item* item = player->inventory.items[startIdx + i];
        String itemName = item->name;

        if (item->isEquipment()) {
            Equipment* equipment = static_cast<Equipment*>(item);
            itemName += equipment->isEquipped ? " (E)" : " (Equip)";
        } else if (item->isConsumable()) {
            itemName += " (Use)";
        }

        tft.setTextColor((startIdx + i) == itemOption ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
        tft.drawString(itemName, 10, menuStartY + i * 20, 1);
    }

    // Add a 'Back' option at the end of the visible list
    if (itemOption == itemCount) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.drawString("Back", 10, menuStartY + (maxDisplayItems) * 20, 1);
    }

    // Wrap-around logic for the itemOption
    if (itemOption < 0) {
        itemOption = itemCount; // Wrap to 'Back' option
    } else if (itemOption > itemCount) {
        itemOption = 0; // Reset to top of the list
    }
}


void Combat::displayCombatStatus(int yPosition) {
    // Only update the display if the value has changed
    if (player->HP != prevPlayerHP || player->MP != prevPlayerMP || player->gold != prevPlayerGold) {
        // Clear the area for the status only if values have changed
        tft.fillRect(0, yPosition, 320, 20, TFT_BLACK);

        // Display player HP and MP status
        tft.setTextColor(TFT_BLUE, TFT_BLACK);
        tft.drawString(String(player->name), 10, yPosition, 1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
         tft.drawString(" HP: " + String(player->HP) + " MP: " + String(player->MP) + " LV: " + String(player->level) + " G: " + String(player->gold), 45, yPosition, 1);
        

        // Update the previous values
        prevPlayerHP = player->HP;
        prevPlayerMP = player->MP;
        prevPlayerGold = player ->gold;
    }
}

void Combat::displaySpellMenu() {
    int menuStartY = 180; // Starting Y position for the spell menu
    tft.fillRect(0, menuStartY, 320, 60, TFT_BLACK); // Clear previous menu
    String spells[] = {"Heal", "Hurt", "Fireball", "Blessing", "Ice Storm", "Thunder"};
    int spellLevels[] = {3, 4, 5, 8, 10, 12};
    int numSpells = sizeof(spells) / sizeof(spells[0]);
    // Determine number of spell options player can select based on level
    int availableSpells = 0;
    for (int i = 0; i < numSpells; i++) {
        if (player->level >= spellLevels[i]) {
            availableSpells++;
        }
    }
    // Include "Back" option in total count
    numSpellOptions = availableSpells + 1;
    // Ensure spellOption does not exceed the total number of spell options
    spellOption = min(spellOption, numSpellOptions);
    int displayedSpells = 0;
    for (int i = 0; i < numSpells && displayedSpells <= spellOption; i++) {
        if (player->level >= spellLevels[i]) {
            if (displayedSpells >= spellOption - MAX_ITEMS_DISPLAY + 1) {
                tft.setTextColor(displayedSpells == spellOption ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
                tft.drawString(spells[i], 10, menuStartY + (displayedSpells - (spellOption - MAX_ITEMS_DISPLAY + 1)) * 20, 1);
            }
            displayedSpells++;
        }
    }
    // Display "Back" option
    if (spellOption == availableSpells) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.drawString("Back", 10, menuStartY + (MAX_ITEMS_DISPLAY - 1) * 20, 1);
    }
    if (spellOption < 0) {
      spellOption = numSpellOptions;
    }
    if (spellOption > numSpellOptions) {
      spellOption = 0;
    }
}

void Combat::update() {
  if(!inCombat) {
    startCombat();
  }
    if (inCombat) {
        handleInput();
        if (playerActionComplete) {
            combatTurn();    
            playerActionComplete = false;           
        }
    }
}

void Combat::render() {
    if (inCombat) {
        renderCombatMenu();
        displayCombatStatus(20); // Assuming this method updates the combat status display
    }
}
