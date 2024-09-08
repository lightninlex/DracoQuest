#include "ConsumableItem.h"
#include "Common.h"

ConsumableItem::ConsumableItem(String name, int potency, String effectType, int price)
    : Item(name, "consumable", price), potency(potency), effectType(effectType) {
    // Constructor implementation
}


        
        
void ConsumableItem::use(Creature& target) {
  int messageY = 0;
    tft.fillRect(0, messageY, 320, 40, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    String message = "You consumed the " + name + "!";
    tft.drawString(message, 10, messageY, 1);
    messageY+=20;
    if (effectType == "heal") {
        target.HP += potency;
        if (target.HP > target.getMaxHp()) {
            target.HP = target.getMaxHp();
        }
        String message = "The fresh greens invigorate your mouth.";
        tft.drawString(message, 10, messageY, 1);
    }
    if (effectType == "mana") {
        target.MP += potency;
        if (target.MP > target.getMaxMp()) {
          target.MP = target.getMaxMp();
        }
        String message = "The viscous fluid burns as you swallow.";
        tft.drawString(message, 10, messageY, 1);
    }
    if (effectType == "attackBuff") {
        target.ATK += potency;
    }
    if (effectType == "defenseBuff") {
        target.DEF += potency;
    }
    delay(800);
    tft.fillRect(0, 0, 320, 80, TFT_BLACK);
}
