#include "Game.h"
#include "Common.h"
#include "ConsumableItem.h"
#include "Equipment.h"
#include "Creature.h"
#include "Inventory.h"
#include "Combat.h"
#include <TFT_eSPI.h> // Include this in Combat.h or Game.h or a common header

Game::Game(Creature* player) : player(player) {
    combatSystem = new Combat(player, enemyManager);
    townSystem = new Town(player);
    exploreSystem = new Explore(player);  // Initialize exploration system
    enemyManager = new EnemyManager(exploreSystem);
    pinMode(32, INPUT_PULLUP);
    pinMode(33, INPUT_PULLUP);
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
    pinMode(39, INPUT_PULLUP);
    setupDisplay();
}


void Game::setupDisplay() {
    tft.init();
    tft.setRotation(3); // Adjust as needed for your display orientation
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // Default text color
}


void Game::setup() {
    // Initialize game components and state
    tft.fillScreen(TFT_BLACK);
    Serial.begin(115200);
    int seed = analogRead(39);
    randomSeed(seed);
    player->MP = player->getMaxMp();
    player->ATK = random(4,10);
    player->DEF = random(4,10);
    player->SPD = random(4,10);
    player->baseHp = random(7,16);
    player->HP = player->getMaxHp();
    townSystem->enterTown();
}

void Game::printLevelUpStats() {
  tft.setTextSize(2);
  tft.setCursor(0,100);
  tft.setTextColor(TFT_BLUE);
  tft.print("HP + "); tft.setTextColor(TFT_GREEN); tft.println(player->HpGained); tft.setTextColor(TFT_BLUE);
  tft.print("MP + "); tft.setTextColor(TFT_GREEN); tft.println(player->MpGained); tft.setTextColor(TFT_BLUE);
  tft.print("ATK + "); tft.setTextColor(TFT_GREEN); tft.println(player->AtkGained); tft.setTextColor(TFT_BLUE);
  tft.print("DEF + "); tft.setTextColor(TFT_GREEN); tft.println(player->DefGained); tft.setTextColor(TFT_BLUE);
  tft.print("SPD + "); tft.setTextColor(TFT_GREEN); tft.println(player->SpdGained);tft.setTextColor(TFT_BLUE);
  if (player->level == 3){
      tft.print("Learned spell: Heal!");
  }
  if (player->level == 4){
    tft.print("Learned spell: Hurt!");
  }
  if (player->level == 5){
    tft.print("Learned spell: Fireball!");
  }
  if (player->level == 8){
    tft.print("Learned spell: Blessing");
  }
  if (player->level == 10){
    tft.print("Learned spell: Ice Storm!");
  }
  if (player->level == 12){
    tft.print("Learned spell: Thunder!");
  }
delay(2500);
tft.fillScreen(TFT_BLACK);
tft.setTextSize(1);
}


void Game::update() {
    if (player->leveling) {
        printLevelUpStats();
        player->leveling = false;
    }
    if (townSystem->isAtTown) {
        townSystem->handleTownInput();
        if (townSystem->isExploring) {
          exploreSystem->isExploring = true;
          townSystem->isExploring = false;
          townSystem->isAtTown = false;
          exploreSystem->zoneX = 1;
          exploreSystem->zoneY = 1;
          exploreSystem->playerX = 3;
          exploreSystem->playerY = 3;
          exploreSystem->startExplore();
          exploreSystem->displayExploreMenu();
        }
    } else if (exploreSystem->isExploring) {
        exploreSystem->exploreUpdate();
         if (exploreSystem && exploreSystem->enteringTown) {
            townSystem->enterTown();
            exploreSystem->enteringTown = false;
        }
        if (exploreSystem->beginCombat) {
           Creature* enemy = enemyManager->generateEnemy(*player);

            if (enemy) {  // Check if enemy is not null
                combatSystem->enemy = enemy;
                combatSystem->startCombat();
                combatSystem->inCombat = true;
            }
          exploreSystem->beginCombat = false;
          exploreSystem->isExploring = false;
        }
    } else if (combatSystem->inCombat) {
        combatSystem->update();
        if (combatSystem->isDead){
        if (combatSystem->enemy) {
      delete combatSystem->enemy;  // Delete the enemy instance
      combatSystem->enemy = nullptr;  // Set enemy to nullptr to avoid using a dangling pointer
    }
        exploreSystem->updateCurrentZoneAndArea();
        exploreSystem->isExploring = true;
        exploreSystem->enteringTown = true;
        exploreSystem->exploreUpdate();
        combatSystem->isDead = false;

      }
    } else if (combatSystem->combatOver) {
      exploreSystem->isExploring = true;
      exploreSystem->exploreUpdate();
      exploreSystem->displayExploreMenu();
      exploreSystem->beginCombat = false;
    if (combatSystem->enemy) {
      delete combatSystem->enemy;  // Delete the enemy instance
      combatSystem->enemy = nullptr;  // Set enemy to nullptr to avoid using a dangling pointer
    }
    }
}

void Game::render() {
    if (townSystem->isAtTown) {
        townSystem->renderTownMenu(0);  // Example, pass current menu option
    } else if (combatSystem->inCombat) {
        combatSystem->render();
    } else if (exploreSystem->isExploring) {
        exploreSystem->displayExploreMenu();
    }
}
