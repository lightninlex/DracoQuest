#include "Game.h"
#include "Common.h"
#include <TFT_eSPI.h>  // Include the library for the display
 TFT_eSPI tft = TFT_eSPI(); // Create a display object
 // Initialize player
 String playerName;
Creature player("Hero", 1, 15, 15, 8, 4, 5, 0, 0, true); // true indicates that the player can cast spells
  // Initialize the game with the player creature
  Game game(&player);

void createPlayer() {
// ask player name
// ask player path
// ask player background
// apply relevant changes
}

void setup() {
  game.setup();
  createPlayer();
  
}

void loop() {
  game.update(); // Process game logic
  game.render(); // Render game state to the display
}