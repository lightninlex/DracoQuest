#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include "Creature.h"
#include "Explore.h"
class EnemyManager {
public:
Explore* exploreSystem;  // Pointer to Explore object

    EnemyManager(Explore* explore);  // Constructor with Explore parameter
    Creature* generateEnemy(Creature& player);
};

#endif
