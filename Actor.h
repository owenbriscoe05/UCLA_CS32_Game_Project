//
//  Actor.hpp
//  CS32_Project3v2
//
//  Created by Owen Briscoe on 5/14/24.
//

#ifndef Actor_hpp
#define Actor_hpp

#include "GameObject.h"
//using namespace std;

const int MAX_ITEMS = 26;

class Actor {
public:
    Actor(string description, string weapon, int hp, int armor, int strength, int dex, int sleep, int init_row, int init_col);
    virtual ~Actor();
    virtual void setRow(int newR);
    virtual void setCol(int newC);
    virtual void increaseRow();
    virtual void decreaseRow();
    virtual void increaseCol();
    virtual void decreaseCol();
    virtual int getHP();
    virtual int getMaxHP();
    virtual int getArmor();
    virtual int getStrength();
    virtual int getDex();
    virtual int getSleep();
    virtual char getItemUnder();
    virtual void setHP(int hp);
    virtual void setMaxHP(int maxHP);
    virtual void setArmor(int armor);
    virtual void setDex(int dex);
    virtual void setStrength(int strength);
    virtual void setSleep(int sleep);
    virtual void setOnItem(bool set);
    virtual void setItemUnder(char c);
    virtual string getDescription();
    virtual string getWeaponName() = 0;
    virtual int row();
    virtual int col();
    virtual bool getOnItem();
    virtual char showChar() = 0;
    virtual void move(Actor* a, char (&grid)[18][70], int row, int col);
    virtual string attack(Actor* a, Actor* b);
    virtual Weapon* currWeap();
    virtual int getHit(int damagePoints);
    virtual void setWeapon(Weapon* w) = 0;
    virtual bool getHasMoved();
    virtual void setHasMoved(bool b);
private:
    string m_description;
    int m_hp;
    int m_max_hp;
    int m_armor;
    int m_strength;
    int m_dex;
    int curr_row;
    int curr_col;
    bool onItem;
    char itemUnder;
    bool hasMoved;
    Weapon* currentWeapon;
    int sleepTime;
};

class Player: public Actor {
public:
    Player() : Actor("player", "short sword", 20, 2, 2, 2, 0, 0, 0) { //utilize the actor's constructor, which is also necessary since Player is a derived class
        for (int i = 0; i < MAX_ITEMS; i++) //initialize the whole inventory to nullptrs to begin with
            inventory[i] = nullptr;
        m_numItems = 0;
        inventory[0] = new ShortSword(); //but also set the first item of the inventory to be a new short sword.
        m_numItems++;
        currentWeapon = dynamic_cast<Weapon*>(inventory[0]); //and intialize player's currentWeapon to inventory[0] (not another dynamically allocated short sword). actor has a currentWeapon, which will be a short sword, but this currentWeapon will be a pointer to a different short sword, which gets rid of the destructor issue where player's destructor deletes an object that actor's destructor also then attempts to delete.
    }
    ~Player();
    char showChar() { return '@';}
    int numItems();
    string pickUpWeapon(string input);
    string pickUpScroll(string input);
    void displayInventory();
    void setWeapon(Weapon* w);
    string getWeaponName();
    Weapon* accessWeapons(int i);
    Scroll* accessScrolls(int i);
    void deleteItemFromInventory(int i);
    void teleport(char (&grid)[18][70]);
private:
    GameObject* inventory[MAX_ITEMS];
    int m_numItems;
    Weapon* currentWeapon;
};

class Bogeyman: public Actor {
public:
    Bogeyman() : Actor("bogeyman", "short sword", randInt(5, 10), 2, randInt(2, 3), randInt(2, 3), 0, 0, 0){}
    ~Bogeyman() {}; //bogeymen and the rest of the monsters don't need any specification for their destructors, but they still need to be defined to ensure they are called, as these are derived classes
    virtual char showChar() { return 'B';}
    void setWeapon(Weapon* w) { //setWeapon is a pure virtual function, so it must be defined
        w = nullptr;
    }
    string getWeaponName() {return "short sword";} //inline implementation, to save some unnecessary code in Actor.cpp
        
};

class Snakewoman: public Actor {
public:
    Snakewoman() : Actor("snakewoman", "magic fangs", randInt(3, 6), 3, 2, 3, 0, 0, 0) {}
    ~Snakewoman() {}
    char showChar() { return 'S';}
    void setWeapon(Weapon* w) {
        w = nullptr;
    }
    string getWeaponName() {return "magic fangs";}
};

class Dragon: public Actor {
public:
    Dragon() : Actor("dragon", "long sword", randInt(20, 25), 4, 4, 4, 0, 0, 0) {}
    ~Dragon() {}
    char showChar() { return 'D';}
    void setWeapon(Weapon* w) {
        w = nullptr;
    }
    string getWeaponName() {return "long sword";}
};

class Goblin : public Actor {
public:
    Goblin(int smellDistance) : Actor("goblin", "short sword", randInt(15, 20), 1, 3, 1, 0, 0, 0) { m_smellDistance = smellDistance;}
    ~Goblin() {}
    char showChar() {return 'G';}
    void move(Goblin* a, char grid[18][70], int row, int col, int &fGobRow, int &fGobCol);
    int moveHelper(Goblin* a, char grid[18][70], bool tempGrid[18][70], int grow, int gcol,  int counter);
    void setWeapon(Weapon* w) {
        w = nullptr;
    }
    int getSmellDistance() {return m_smellDistance;}
    string getWeaponName() {return "short sword";}
private:
    int m_smellDistance;
    
};

char surroundingsCheck(Actor* a, char (&grid)[18][70], int row, int col);


#endif /* Actor_hpp */
