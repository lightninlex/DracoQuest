// Creature.cpp
#include "Creature.h"
#include "Common.h"
#include <TFT_eSPI.h>


/*

weapon bonus defined by weapon quality/sharpness
Each strike decrements sharpness by random(1,6)
need whetstone item

// monster loot/parts/crafting?
restore armor points at SMITH in town // create special inventory index for broken armor



introduce effects for spells/weapons
BURN(combat), STUN(combat), BLEED(oustide), POISON(outside)
add resistances for armor/acc

*/




int fleeAttempts = 0;

void Creature::castSpell(String spellName, Creature &target) {
    if (!canCastSpells) return;
    // modify to account for player path

    if (spellName == "Heal" && MP >= 3) {
        int healAmount = random(10, 26);
        HP += healAmount;
        MP -= 3;
    } else if (spellName == "Hurt" && MP >= 4) {
        int damage = random(10, 21);
        target.HP -= damage;
        MP -= 4;
    } else if (spellName == "Fireball" && MP >= 6) {
        int damage = random(30, 46);
        target.HP -= damage;
        MP -= 6;
    } else if (spellName == "Blessing" && MP >= 8) {
        int healAmount = random(20, 41);
        HP += healAmount; 
        MP -= 8;
    } else if (spellName == "Ice Storm" && MP >= 10) {
        int damage = random(45, 56);
        target.HP -= damage;
        MP -= 10;
    } else if (spellName == "Thunder" && MP >= 12) {
        int damage = random(55, 66);
        target.HP -= damage;
        MP -= 12;
    }

    if (HP > getMaxHp()) {
        HP = getMaxHp();
    }
    if (MP > getMaxMp()) {
        MP = getMaxMp();
    }
}

void Creature::levelUp() {
  // modify to account for player path
   prevAtk = ATK;
   prevDef = DEF;
   prevSpd = SPD;
    ATK += random(2, 6);
    DEF += random(1, 6);
    SPD += random(1, 4);
    HP = getMaxHp(); //restore HP and MP to reset growth markers
    MP = getMaxMp();
    prevHp = HP;
    prevMp = MP;
    level++;
    HP = getMaxHp(); // apply level up hp and mp
    MP = getMaxMp();
    HpGained = HP - prevHp; // calculate growth markers
    MpGained = MP - prevMp;
    AtkGained = ATK - prevAtk;
    DefGained = DEF - prevDef;
    SpdGained = SPD - prevSpd;
}

void Creature::checkLevelUp() {
    // Continue checking if the player has enough XP to level up
    while (xp >= requiredXP && level < maxLevel) {
        // Subtract the required XP for the current level
        xp -= requiredXP;
        if (level<11){
          requiredXP *= 2; // This will double the XP requirement each time a level is gained
        } else {
          requiredXP += (requiredXP/2);
        }
        levelUp();
        leveling = true;
    }
}

void Creature::combatAction(Creature &target) {
    if (name == "Dracky") {
        if (HP < getMaxHp()/2) {
            MP >= 4 ? castSpell("Hurt", target) : attack(target);
        } else {
            attack(target);
        }
    } else if (name == "Ghost") {
        if (MP >= 6 && level > 10 && HP < getMaxHp() * .5) {
            castSpell("Hurt", target);
        } else {
            attack(target);
        }
    } else if (name == "SkeletonKnight") {
        if (HP < getMaxHp() * .5 && MP >= 6) {
            castSpell("Fireball", target);
        } else if (HP < getMaxHp() * .3 && MP >= 3) {
            castSpell("Heal", *this);
        } else {
            attack(target);
        }
    } else if (name == "Necromancer") {
        if (MP >= 8 && HP < getMaxHp() * .4) {
            castSpell("Blessing", *this);
        } else {
            attack(target);
        }
    } else if (name == "Dragon Lord") {
        if (HP < getMaxHp() * 0.50 && MP >= 12) {
            castSpell("Thunder", target);
        } else if (HP < getMaxHp() * .4 && MP >= 8) {
            castSpell("Fireball", target);
        } else {
            attack(target);
        }
    } else {
        attack(target);
    }
}



bool Creature::tryToFlee(Creature &enemy) {
  // modify to account for player speed
    float fleeChance = 50 + (fleeAttempts * 10) + ((SPD - enemy.SPD) * 5); // Base chance + speed difference adjustment
    if (fleeChance > random(100)) {
        return true;
    } else {
        return false;
    }
    fleeAttempts++;
}

bool Creature::shouldEnemyFlee(Creature &player) {
    if (HP < getMaxHp() * 0.25 || ATK <= player.ATK/4) {  // Check if HP is below 25% or if overpowered
        float fleeChance = 50 + ((SPD - player.SPD) * 5);  // Base flee chance plus speed modifier
        return random(100) < fleeChance;
    }
    return false;
}

void Creature::attack(Creature &target) {
  // modify to account for player path and player speed
    int critChance = random(0, 101);
    float damageMultiplier = critChance >= 75 ? random(1.6, 2.3) : random(1.0, 1.3);
    if (critChance >= 75) {
      criticalHit = true;
    }
    // Level-based multipliers
    float attackerLevelMultiplier = 1 + (level * 0.10); // 5% increase per level for attacker
    float defenderLevelMultiplier = 1 + (target.level * 0.10); // 5% increase per level for defender

    float attackValue = static_cast<float>(ATK) * attackerLevelMultiplier;
    float defenseValue = (target.DEF + 10) * defenderLevelMultiplier;
    
    float attackFactor = attackValue / defenseValue;
    int power = 10;  // Base power of the attack, can be adjusted

    int calculatedDamage = static_cast<int>(attackFactor * power * damageMultiplier);
    int damage = calculatedDamage >= 3 ? calculatedDamage : random(1, 6);  // Ensures minimum damage is 5

    target.HP -= damage;
    lastAttackDamage = damage;
}




bool Creature::canCastSpell(const String& spellName) {
    if (!canCastSpells) return false;
    // modify to account for player path

    if (spellName == "Heal") {
        return level >= healSpellLevelRequirement;
    } else if (spellName == "Hurt") {
        return level >= hurtSpellLevelRequirement;
    } else if (spellName == "Fireball") {
        return level >= fireballSpellLevelRequirement;
    } else if (spellName == "Blessing") {
        return level >= blessingSpellLevelRequirement;
    } else if (spellName == "Ice Storm") {
        return level >= iceStormSpellLevelRequirement;
    } else if (spellName == "Thunder") {
        return level >= thunderSpellLevelRequirement;
    }

    return false; // If the spell name doesn't match known spells
}

