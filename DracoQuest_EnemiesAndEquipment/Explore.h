//Explore.h

#ifndef EXPLORE_H
#define EXPLORE_H

#include "Creature.h"
#include "Common.h"
#include "Inventory.h"
#include "ConsumableItem.h"
#include "Equipment.h"
#include <TFT_eSPI.h>  // Assuming use of the TFT display for rendering

class Explore {
private:
    int numItemOptions;
    int topItemIndex = 0;  // Tracks the top item index displayed in the item menu
    int itemMenuOffset = 0;  // Add this to the class members
    const int MAX_ITEMS_DISPLAY = 3;
   static const int ZONE_WIDTH = 4;
    static const int ZONE_HEIGHT = 4;
    Creature* player;
    int areaX, areaY;  // Player's position within the current zone's 3x3 area
    int area;
    int zone;
    int zones[ZONE_HEIGHT][ZONE_WIDTH] = {
      {1,2,3,4},
      {5,6,7,8},
      {9,10,11,12},
      {13,14,15,16}
    };  // make 10x10

static const int AREA_SIZE = 3;  // Assuming each zone is a 3x3 grid of areas
int areas[AREA_SIZE][AREA_SIZE] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
};    //make 5x5
    int currentZone;  // ID of the current zone
    int currentArea;  // ID of the current area within the zone
    bool discoveryFound;
    bool foundTreasure;
    bool inCombat;

int zoneTypes[ZONE_HEIGHT][ZONE_WIDTH] = {
      {1, 1, 2, 2},
      {1, 0, 0, 2},
      {1, 0, 0, 2},
      {1, 1, 2, 2}
    }; // create 8 zone types.
    // grassland, forest, mountain, swamp, poison marsh, fae forest, rocky cliffs, badlands
    int selectedItemIndex;
    int screenWidth = 320;
    int screenHeight = 240;
    int menuHeight = 60;
    int statusHeight = 20;
    int menuStartY = screenHeight - menuHeight;
    int statusStartY = menuStartY - statusHeight;
    int messageY = 0;
public:
    static const int AREA_WIDTH = 3;
    static const int AREA_HEIGHT = 3;
    int exploreOption = -1;
    int prevExploreOption = -1;
    int townAreaX = 1; // second column in the zone for area 2
    int townAreaY = 0; // first row in the zone
    int townZoneX, townZoneY; // Town's location on the world map
    int playerX, playerY;  // Player's position in the exploration grid(Area)
    int zoneX, zoneY;  // Player's current zone in the 4x4 world map
    int zoneType;    // Type of the current zone
    bool beginExplore = false;
    bool inItemMenu;
    bool beginCombat = false;
    bool inOverworld;
    bool inDiscovery;
    bool enteringTown;
    bool isExploring;
    int itemOption;
    Explore(Creature* player);  // Constructor that takes the player character
    void performExploreAction();
    void handleInput();
    void exploreUpdate();
    void exploreRender();
    void resolveExplore();
    void chanceDiscovery();
    void discoveryResult();
    void getZone();
    void getArea();
    void displayExploreMenu();
    void displayExploreResolve();
    void updateCurrentZoneAndArea();
    void getTreasure();
    void startExplore();
    // Additional methods for navigation
    void moveNorth();
    void moveSouth();
    void moveEast();
    void moveWest();
    void useItem();  // If using an item is an option during exploration
};

#endif
