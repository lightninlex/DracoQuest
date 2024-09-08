// Creature.h
#ifndef CREATURE_H
#define CREATURE_H
#include <Arduino.h>
#include "Inventory.h"

class Creature {

// add resistance and weakness
// add spell mastery
// add equipment mastery
  private:
  const int thunderSpellLevelRequirement = 12;
  const int iceStormSpellLevelRequirement = 10;
  const int blessingSpellLevelRequirement = 8;
  const int fireballSpellLevelRequirement = 5;
  const int healSpellLevelRequirement = 3;
  const int hurtSpellLevelRequirement = 4;
public:
    Inventory inventory;
    int HP, MP, ATK, DEF, SPD, baseHp, baseMp, lastAttackDamage;
    String name;
    int level, gold, xp, maxLevel;
    
    int fleeAttempts;
    bool canCastSpells; 
    bool criticalHit = false;
    bool leveling = false;
    int prevHp;
    int prevMp;
    int prevAtk;
    int prevDef;
    int prevSpd;
    int HpGained;
    int MpGained;
    int AtkGained;
    int DefGained;
    int SpdGained;
    Creature(String name, int level, int baseHp, int baseMp, int baseAtk, int baseDef, int baseSpd, int gold, int xp, bool canCastSpells)
    : name(name), level(level), baseHp(baseHp), baseMp(baseMp), HP(baseHp + level * 2), MP(baseMp),
      ATK(baseAtk), DEF(baseDef), SPD(baseSpd), gold(gold), xp(xp), maxLevel(20),
      canCastSpells(canCastSpells), criticalHit(false), leveling(false) {}

    int getMaxHp() const {
        return baseHp + (level * 2);
    }

    int getMaxMp() const {
    if (level < 3){
      return baseMp *0;
    } else {
        return baseMp + (level*2);
    }
    }
    bool canCastSpell(const String& spellName);
    bool shouldEnemyFlee(Creature &player);  // Add this in the Creature class declaration
    void combatAction(Creature &target);
    void attack(Creature &target);
    void castSpell(String spellName, Creature &target);
    bool tryToFlee(Creature &enemy);
    int requiredXP = 4;
    void levelUp();
    void checkLevelUp();
};


#endif // CREATURE_H
