#include "EnemyManager.h"
// modify to include new zone types and time of day

#include "Creature.h"  // Ensure Creature is included if not already
EnemyManager::EnemyManager(Explore* explore) : exploreSystem(explore) {}
Creature* EnemyManager::generateEnemy(Creature& player) {
    int playerLevel = player.level;
    int zoneType = exploreSystem->zoneType;

    // Check for zone type 2 first, as it's independent of player level
    if (zoneType == 2) {
        if (playerLevel > 10) {
            // Fourth set of enemies, for players above level 10 in zone type 2
            int enemyType = random(0, 11);
            switch (enemyType) {
          case 0:
            return new Creature("Metal Slime", 8, 250, 0, 10, 25, 13, 200, 200, false);
          case 1:
            return new Creature("Drakeus", 8, 15, 10, 19, 16, 16, 24, 24, true);
          case 2:
            return new Creature("Spectre", 10, random(25, 31), 20, 21, 25, 28, 50, 50, true);
          case 3:
            return new Creature("Scorpion", 12, random(25, 46), 0, 31, 35, 38, 70, 50, true);
          case 4:
            return new Creature("WolfLord", 15, random(31, 41), 0, random(35, 46), 25, 28, 85, 80, true);
          case 5:
            return new Creature("SkeletonLord", 16, random(30, 46), 15, random(15, 20), 25, 28, 60, 80, true);
          case 6:
            return new Creature("Dragon Lord", 20, random(150, 201), 0, 50, 20, 15, 400, 5000, true);
          case 7:
            return new Creature("Necromancer", 15, random(100, 151), 20, 30, 25, 20, 300, 100, true);
          case 8:
            return new Creature("Ice Sorceress", 18, random(120, 180), 15, 35, 22, 18, 250, 200, true);
          case 9:
            return new Creature("Demon Warlord", 22, random(201, 255), 0, 45, 30, 25, 350, 400, true);
          case 10:
            return new Creature("Giant Stone Golem", 250, 5, 0, 60, 10, 30, 500, 5, false);
          
          default:
            // Fallback to a high-level enemy if random number exceeds range
            return new Creature("Dark Wizard", 18, random(15, 20), 25, 40, 30, 20, 35, 40, true);
    }
        } else {
            // Third set of enemies, for any player level in zone type 2
            int enemyType = random(0, 8);
            switch (enemyType) {
        
          case 0:
            return new Creature("Slime", 8, 15, 0, 15, 15, 13, 20, 20, false);
          case 1:
            return new Creature("Dracky", 8, 15, 10, 19, 16, 16, 24, 24, true);
          case 2:
            return new Creature("Ghost", 10, random(25, 31), 20, 21, 25, 28, 25, 25, false);
          case 3:
            return new Creature("Scorpion", 12, random(25, 46), 0, 31, 35, 38, 40, 45, false);
          case 4:
            return new Creature("BewareWolf", 15, random(31, 41), 0, random(35, 46), 25, 28, 55, 55, true);
          case 5:
            return new Creature("SkeletonKnight", 16, random(30, 46), 15, random(15, 20), 25, 28, 30, 30, true);
          case 6:
            return new Creature("Necromancer", 16, random(100, 151), 18, 25, 25, 20, 300, 85, true);
          case 7:
            return new Creature("Ice Sorceress", 17, random(120, 180), 12, 35, 22, 18, 250, 300, true);
          default:
            // Fallback to a high-level enemy if random number exceeds range
            return new Creature("Dark Wizard", 18, random(15, 20), 25, 40, 30, 20, 35, 40, true);
    }
        }
    } else {
        // Conditions for zone types 0 and 1
        if (playerLevel < 5 || zoneType == 0) {
            // First set of enemies, for players below level 5 or in zone type 0
            int enemyType = random(0, 5);
            switch (enemyType) {
          case 0:
            return new Creature("Slime", 1, 3, 0, 5, 5, 3, 2, 2, false);
          case 1:
            return new Creature("Red Slime", 2, 4, 0, 7, 3, 3, 4, 4, false);
          case 2:
            return new Creature("Magician", 5, 13, 5, 11, 12, 12, 11, 4, true);
          case 3:
            return new Creature("Dracky", 5, 1, 10, 9, 6, 6, 8, 6, true);
          case 4:
            return new Creature("Ghost", 1, random(8, 10), 10, 11, 5, 8, 5, 5, false);
          default:
            return new Creature("Slime", 1, 3, 0, 5, 5, 3, 2, 2, false);
        }
        } else if (playerLevel <= 10) {
            // Second set of enemies, for players between level 5 and 10 (inclusive)
            int enemyType = random(0, 6);
            switch (enemyType) {
            case 0:
                return new Creature("Slime", 5, 10, 0, 8, 9, 6, 6, 16, false);
            case 1:
                return new Creature("Dracky", 8, 20, 10, 13, 6, 6, 9, 19, true);
            case 2:
                return new Creature("Ghost", 7, random(10, 16), 10, 15, 5, 8, 7, 17, true);
            case 3:
                return new Creature("Scorpion", 6, random(10, 16), 0, 11, 5, 8, 10, 20, false);
            case 4:
                return new Creature("BewareWolf", 9, random(15, 21), 0, random(15, 31), 15, 8, 25, 35, false);
            case 5:
                return new Creature("SkeletonKnight", 8, random(20, 26), 15, random(15, 20), 5, 8, 20, 30, false);
            default:
                return new Creature("Ghost", 3, random(8, 10), 10, 11, 5, 8, 15, 15, true);
        }
        }
    }

    return new Creature("Default Enemy", 1, 1, 0, 1, 1, 1, 1, 1, false); // Default case
}
/*


("Wolf", 10, 34, 0, 40, 30, 34, 49, 16, false)
("Druin", 7, 22, 0, 20, 18, 18, 15, 7, false)



*/
