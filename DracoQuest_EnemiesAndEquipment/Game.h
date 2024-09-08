#ifndef GAME_H
#define GAME_H

#include "EnemyManager.h"
#include "Creature.h"
#include "Combat.h"
#include "Town.h"
#include "Explore.h"  // Include Explore class
#include <Arduino.h>
#include <TFT_eSPI.h>


class Game {
public:
    EnemyManager* enemyManager;
    Creature* player;
    Combat* combatSystem;
    Town* townSystem;
    Explore* exploreSystem;  // Add the Explore attribute

    Game(Creature* player);
    void setupDisplay();
    void setup();
    void update();
    void render();
    void printLevelUpStats();
};

#endif // GAME_H
