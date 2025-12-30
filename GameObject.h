//
//  GameObject.hpp
//  CS32_Project3v2
//
//  Created by Owen Briscoe on 5/18/24.
//

#ifndef GameObject_hpp
#define GameObject_hpp

#include <string>
#include "utilities.h"
using namespace std;

class GameObject {
public:
    GameObject(string description, string actionString) {m_description = description; m_actionString = actionString;}
    virtual ~GameObject() {}
    virtual void setRow(int newR);
    virtual void setCol(int newC);
    virtual int row();
    virtual int col();
    virtual string getDescription();
private:
    string m_description;
    string m_actionString;
    int m_col;
    int m_row;
};

class Weapon : public GameObject {
public:
    Weapon(string description, int dexBonus, int damagePoints) : GameObject(description, "You swing ") {m_dexBonus = dexBonus; m_damagePoints = damagePoints; m_description = description;}
    int dexBonus() {return m_dexBonus;}
    int damagePoints() {return m_damagePoints;}
private:
    string m_description;
    int m_dexBonus;
    int m_damagePoints;
};

class Scroll : public GameObject {
public:
    Scroll(string description, int improveAmount) : GameObject(description, "You read ") {
        m_improveAmt = improveAmount;
    }
    int getImproveAmt();
private:
    int m_improveAmt;
};

class Mace : public Weapon {
public:
    Mace() : Weapon("Mace", 0, 2) {}
};

class ShortSword : public Weapon {
public:
    ShortSword() : Weapon("Short sword", 0, 2) {}
};

class LongSword : public Weapon {
public:
    LongSword() : Weapon("Long sword", 2, 4) {}
};

class MagicAxe : public Weapon {
public:
    MagicAxe() : Weapon("Magic axe", 5, 5) {}
};

class MagicFangs : public Weapon {
public:
    MagicFangs() : Weapon("Magic fangs", 3, 2) {sleepTime = randInt(2, 6);}
private:
    int sleepTime;
};

class STeleport : public Scroll {
public:
    STeleport() : Scroll("Scroll of teleportation", 0) {}
};

class SArmor : public Scroll {
public:
    SArmor() : Scroll("Scroll of improve armor", randInt(1, 3)) {}
private:
    
};

class SStrength : public Scroll {
public:
    SStrength() : Scroll("Scroll of raise strength", randInt(1, 3)) {}
private:
};

class SHealth : public Scroll {
public:
    SHealth() : Scroll("Scroll of enhance health", randInt(3, 8)) {}
    
};

class SDex : public Scroll {
public:
    SDex() : Scroll("Scroll of enhance dexterity", 1) {}
};



#endif /* GameObject_hpp */
