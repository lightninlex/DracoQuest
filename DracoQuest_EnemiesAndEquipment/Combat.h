#ifndef COMBAT_H
#define COMBAT_H

#include "EnemyManager.h"
#include "Creature.h"
#include <TFT_eSPI.h> // Include this in Combat.h or Game.h or a common header

class Combat {
private:
    Creature* player;
    int combatOption;
    bool inSpellMenu;
    int spellOption;
    int prevCombatOption; // Initialize to an invalid value to ensure initial drawing
    int prevPlayerHP = -1; // Same for player HP
    int prevEnemyHP = -1; // And enemy HP
    int prevPlayerMP = -1; // Add this to track the player's MP
    int prevSpellOption = -1; // Tracks the previous spell option for display updates
    int prevPlayerGold = -1;
    int numSpellOptions;  // Add this line
    int numItemOptions;
    int topItemIndex = 0;  // Tracks the top item index displayed in the item menu
    int itemMenuOffset = 0;  // Add this to the class members
    const int MAX_ITEMS_DISPLAY = 3;
    int screenWidth = 320;
    int screenHeight = 240;
    int menuHeight = 60;
    int statusHeight = 20;
    int menuStartY = screenHeight - menuHeight;
    int statusStartY = menuStartY - statusHeight;
public:
 Combat(Creature* player, EnemyManager* enemyManager);

    Creature* enemy;
    bool inCombat = false;
    bool playerActionComplete;
    bool inItemMenu;
    bool combatOver = false;
    bool isDead = false;
    EnemyManager* enemyManager; // Add the EnemyManager attribute
    void startCombat();
    void handleInput();
    void performPlayerAction();
    void combatTurn();
    void resolveCombat();
    void renderCombatMenu();
    void displayCombatStatus(int yPosition);
    void displaySpellMenu();
    void displayItemMenu();
    void update();
    void render();
    int itemOption;  // Add this line for item selection
    unsigned long lastInputTime = 0;
    const unsigned long debounceDelay = 150; // debounce time in milliseconds
};

#endif // COMBAT_H
