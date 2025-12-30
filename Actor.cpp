//
//  Actor.cpp
//  CS32_Project3v2
//
//  Created by Owen Briscoe on 5/14/24.
//

#include "Actor.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <climits>
#include <cstring>
using namespace std;

Actor::Actor(string description, string weapon, int hp, int armor, int strength, int dex, int sleep, int init_row, int init_col) {
    //initialize currentWeapon based on the 3 different weapons a monster/player might begin with
    if (weapon == "short sword")
        currentWeapon = new ShortSword();
    else if (weapon == "long sword")
        currentWeapon = new LongSword();
    else if (weapon == "magic fangs")
        currentWeapon = new MagicFangs();
    m_description = description;
    m_hp = hp;
    m_max_hp = hp;
    m_armor = armor;
    m_strength = strength;
    m_dex = dex;
    curr_row = init_row;
    curr_col = init_col;
    onItem = false;
    hasMoved = false;
    itemUnder = ' ';
    sleepTime = sleep;
}

Actor::~Actor() { //currentWeapon is the only dynamically allocated object that an ACTOR as part of the base class may have to deal with
    delete currentWeapon;
}

//the player, though, also has an inventory of game objects that need to be deleted. The currentWeapon must be somehow changed first, so as not to attempt deleting it twice, as this will cause a bad access error upon game destruction
Player::~Player() {
    Player::currentWeapon = dynamic_cast<Weapon*>(inventory[0]); //so you dynamically cast the currentweapon to inventory[0]
    inventory[0] = nullptr; //and ensure there is no dangling pointer
    
    //and then only delete inventory items beginning at pos 2, so that the weapon that currentWeapon is now pointing to can be deleted by actor's destructor when it is called
    for (int i = 1; i < m_numItems; i++) {
        delete inventory[i];
    }
}

//tons of set/accessor functions
int Actor::row() {
    return curr_row;
}
int Actor::col() {
    return curr_col;
}

void Actor::setRow(int newR) {
    curr_row = newR;
}

void Actor::setCol(int newC) {
    curr_col = newC;
}

void Actor::increaseCol() {
    curr_col++;
}

void Actor::increaseRow() {
    curr_row++;
}

void Actor::decreaseCol() {
    curr_col--;
}

void Actor::decreaseRow() {
    curr_row--;
}

int Actor::getHP() {
    return m_hp;
}

int Actor::getMaxHP() {
    return m_max_hp;
}

int Actor::getDex() {
    return m_dex;
}

int Actor::getArmor() {
    return m_armor;
}

bool Actor::getHasMoved() {
    return hasMoved;
}

void Actor::setHasMoved(bool b) {
    hasMoved = b;
}

int Actor::getStrength() {
    return m_strength;
}

int Actor::getSleep() {
    return sleepTime;
}

int Player::numItems() {
    return m_numItems;
}

void Actor::setHP(int hp) { //only update hp if it won't end up being higher than maxHP
    if (hp <= m_max_hp)
        m_hp = hp;
}

//no max characteristic can be higher than 99
void Actor::setMaxHP(int maxHP) {
    if (maxHP > 99)
        m_max_hp = 99;
    else m_max_hp = maxHP;
}

void Actor::setStrength(int strength) {
    if (strength <= 99)
        m_strength = strength;
    else m_strength = 99;
}

void Actor::setArmor(int armor) {
    if (armor <= 99)
        m_armor = armor;
    else m_armor = 99;
}

void Actor::setDex(int dex) {
    if (dex <= 99)
        m_dex = dex;
    else m_dex = 99;
}

void Actor::setSleep(int sleep) {
    sleepTime = sleep;
}


Weapon* Actor::currWeap() {
    return currentWeapon;
}

string Actor::getDescription() {
    return m_description;
}

string Player::getWeaponName() {
    return currentWeapon->getDescription();
}

bool Actor::getOnItem() {
    return onItem;
}

void Actor::setOnItem(bool set) {
    onItem = set;
}

char Actor::getItemUnder() {
    return itemUnder;
}

void Actor::setItemUnder(char c) {
    itemUnder = c;
}

void Player::setWeapon(Weapon* w) {
    Player::currentWeapon = w;
}

int Actor::getHit(int damagePoints) {
    return (m_hp - damagePoints);
}


void Player::teleport(char (&grid)[18][70]) {
    //similar to player's spawn function
    int prevRow = row();
    int prevCol = col();
    int newRow = 0;
    int newCol = 0;
    while(grid[newRow][newCol] != ' ') {
        newRow = randInt(1, 16);
        newCol = randInt(1, 68);
    }
    setRow(newRow);
    setCol(newCol);
    grid[prevRow][prevCol] = ' ';
    grid[newRow][newCol] = '@';
}

Weapon* Player::accessWeapons(int i) {
    Weapon* temp = dynamic_cast<Weapon*>(inventory[i]); //since inventory is an array of pointers to game objects, we need to dynamically cast so that temp receives a pointer to a weapon
    if (temp != nullptr) { //if dynamic casting was unsuccessful, we don't want to update the inventory
        inventory[i] = Player::currentWeapon;
        Player::currentWeapon = nullptr; //I handle the allocation of temp to currentWeapon in Game.cpp, to make it more explicitly obvious
    }
    return temp;
}

Scroll* Player::accessScrolls(int i) {
    Scroll* temp = dynamic_cast<Scroll*>(inventory[i]); //similar to accessing a weapon
    return temp;
}

//this function is similar to deleting a monster when it is killed
void Player::deleteItemFromInventory(int i) {
    delete inventory[i];
    inventory[i] = nullptr;
    for (int j = i; j < m_numItems; j++) {
        if (j+1 == m_numItems)
            inventory[i] = nullptr;
        else inventory[i] = inventory[i+1];
            
    }
    m_numItems--;
}

void Player::displayInventory() {
    char c = 'a';
    //this is the same as the character increment in game.cpp, but vice versa
    for (int i = 0; i < m_numItems; i++) {
        cout << "Item " << c << ": " << inventory[i]->getDescription() << endl;
        c++;
    }
}

string Player::pickUpWeapon(string input) {
    //if your inventory is full, immediately return
    if (m_numItems > MAX_ITEMS)
        return "full";
    //if input is axe or fangs, that means you are standing on the dropped weapon of a monster, and that weapon should be picked up
    if (input == "axe") {
        inventory[m_numItems] = new MagicAxe();
        m_numItems++;
        return "magic axe";
    }
    else if (input == "fangs") {
        inventory[m_numItems] = new MagicFangs();
        m_numItems++;
        return "magic fangs";
    }
    //ohterwise, randomly generate a weapon to pick up
    int randWeapon = randInt(1, 3);
    switch(randWeapon) {
        case 1:
            inventory[m_numItems] = new Mace();
            m_numItems++;
            return "mace";
            break;
        case 2:
            inventory[m_numItems] = new ShortSword();
            m_numItems++;
            return "short sword";
            break;
        case 3:
            inventory[m_numItems] = new LongSword();
            m_numItems++;
            return "long sword";
            break;
    }
    return "";
}

string Player::pickUpScroll(string input) {
    if (m_numItems > MAX_ITEMS)
        return "full";
    int randWeapon = 0;
    //if input is drag, that means that you are on the dropped scroll of a dead dragon. thus, any scroll is fair game, including scroll of Teleportation, so randWeapon should be from 1 through 5, inclusive
    if (input == "drag")
        randWeapon = randInt(1, 5);
    else { //otherwise randWeapon should be from 2 through 5, inclusive
        randWeapon = randInt(2, 5);
    }
    switch(randWeapon) {
        case 1:
            inventory[m_numItems] = new STeleport();
            m_numItems++;
            return "st";
            break;
        case 2:
            inventory[m_numItems] = new SDex();
            m_numItems++;
            return "sd";
            break;
        case 3:
            inventory[m_numItems] = new SArmor();
            m_numItems++;
            return "sa";
            break;
        case 4:
            inventory[m_numItems] = new SHealth();
            m_numItems++;
            return "sh";
            break;
        case 5:
            inventory[m_numItems] = new SStrength();
            m_numItems++;
            return "ss";
            break;
    }
    return "";
}


//function used in actor::move
char surroundingsCheck(Actor* a, char (&grid)[18][70], int row, int col) {
    if (grid[row][col] != '#' && grid[row][col] != '@' && grid[row][col] != 'B' && grid[row][col] != 'S' && grid[row][col] != 'G' && grid[row][col] != 'D') {
        if (grid[row][col] == ')' || grid[row][col] == '?' || grid[row][col] == '>' || grid[row][col] == '&')
            return grid[row][col]; //return the item character
        else return 'n'; //n represents no obstacle
    }
    
    return 'b'; //b represents block
}

//both the player and all monsters use this function, since player and monster are both derived from the actor base class
string Actor::attack(Actor* a, Actor* b) { //Actor a is the attacker, b is the defender
    bool sleep = false;
    int attackPoints = a->getDex() + a->currentWeapon->dexBonus();
    int defendPoints = b->getDex() + b->getArmor();
    if (randInt(1, attackPoints) >= randInt(1, defendPoints)) { //if the defender gets hit
        if (a->currentWeapon->getDescription() == "Magic fangs") { //if the weapon is fangs
            if (randInt(1, 5) == 1) { //if the fangs put the defender to sleep
                if (b->sleepTime == 0) { //if sleep time is 0
                    b->sleepTime = randInt(2, 6); //use the normal randInt generation
                    sleep = true;
                }
                //otherwise, only set a new sleeptime if a higher time is generated
                else {
                    int temp = randInt(2, 6);
                    if (b->sleepTime < temp)
                        b->sleepTime = temp;
                    sleep = true;
                }
                
            }
        }
        int damagePoints = randInt(0, a->getStrength() + a->currentWeapon->damagePoints());
        int result = b->getHit(damagePoints);
        b->setHP(result);
        if (result <= 0 && b->getDescription() != "player") //meaning a monster got killed
            return "kill";
        else if (b->getDescription() == "player" && b->getHP() <= 0) //meaning the player got killed
            return "dead";
        else if (!sleep) {
            return "hit";
        }
        else return "hit, sleep";
    }
    else return "miss";
}

void Actor::move(Actor* a, char (&grid)[18][70], int row, int col) { //function used by both Bogeymen and Snakewomen -- row and col represent the player's position
        if (a->col() > col) { //first of four checks for relative location to the player
            char c = surroundingsCheck(a, grid, a->row(), a->col() - 1);
            if (c == 'n') { //if the direction is completely unblocked
                a->setCol(a->col() - 1);
                if (getOnItem()) { //if the monster is on an item, then after moving the monster will not be on an item, but the previous location should be updated accordingly
                    grid[a->row()][a->col() + 1] = getItemUnder();
                    a->setOnItem(false);
                    a->setItemUnder(' ');
                }
                else { //if the monster isn't on an item, no worries
                    grid[a->row()][a->col() + 1] = ' ';
                }
                grid[a->row()][a->col()] = a->showChar();
                hasMoved = true; //monster has moved, it cannot attack now
                return;
            }
            //otherwise if the direction has an item
            else if (c == '?' || c == ')' || c == '>' || c == '&'){
                a->setCol(a->col() - 1);
                if (getOnItem()) { //if the monster is already on an item, too
                    grid[a->row()][a->col() + 1] = getItemUnder();
                    a->setOnItem(false);
                    a->setItemUnder(' ');
                }
                else {
                    grid[a->row()][a->col() + 1] = ' ';
                }
                //update to indicate that the monster is stepping onto an item again, and update the itemUnder member to show as such
                grid[a->row()][a->col()] = a->showChar();
                a->setOnItem(true);
                a->setItemUnder(c);
                hasMoved = true;
                return;
            }
        }
    //each other if statement does the same thing as the last one, but with a different direction in mind
        if (a->col() < col) {
            char c = surroundingsCheck(a, grid, a->row(), a->col() + 1);
            if (c == 'n') {
                a->setCol(a->col() + 1);
                if (getOnItem()) {
                    grid[a->row()][a->col() - 1] = getItemUnder();
                    a->setOnItem(false);
                    a->setItemUnder(' ');
                }
                else {
                    grid[a->row()][a->col() - 1] = ' ';
                }
                grid[a->row()][a->col()] = a->showChar();
                hasMoved = true;
                return;
            }
            else if (c == '?' || c == ')'){
                a->setCol(a->col() + 1);
                if (getOnItem()) {
                    grid[a->row()][a->col() - 1] = getItemUnder();
                    a->setOnItem(false);
                    a->setItemUnder(' ');
                }
                else {
                    grid[a->row()][a->col() - 1] = ' ';
                }
                grid[a->row()][a->col()] = a->showChar();
                a->setOnItem(true);
                a->setItemUnder(c);
                hasMoved = true;
                return;
            }
        }
        if (a->row() > row) {
            char c = surroundingsCheck(a, grid, a->row() - 1, a->col());
            if (c == 'n') {
                a->setRow(a->row() - 1);
                if (getOnItem()) {
                    grid[a->row() + 1][a->col()] = getItemUnder();
                    a->setOnItem(false);
                    a->setItemUnder(' ');
                }
                else {
                    grid[a->row() + 1][a->col()] = ' ';
                }
                grid[a->row()][a->col()] = a->showChar();
                hasMoved = true;
                return;
            }
            else if (c == '?' || c == ')') {
                a->setRow(a->row() - 1);
                if (getOnItem()) {
                    grid[a->row() + 1][a->col()] = getItemUnder();
                    a->setOnItem(false);
                    a->setItemUnder(' ');
                }
                else {
                    grid[a->row() + 1][a->col()] = ' ';
                }
                grid[a->row()][a->col()] = a->showChar();
                a->setOnItem(true);
                a->setItemUnder(c);
                hasMoved = true;
                return;
            }
        }
        if (a->row() < row) {
            char c = surroundingsCheck(a, grid, a->row() + 1, a->col());
            if (c == 'n') {
                a->setRow(a->row() + 1);
                if (getOnItem()) {
                    grid[a->row() - 1][a->col()] = getItemUnder();
                    a->setOnItem(false);
                    a->setItemUnder(' ');
                }
                else {
                    grid[a->row() - 1][a->col()] = ' ';
                }
                grid[a->row()][a->col()] = a->showChar();
                hasMoved = true;
                return;
            }
            else if (c == '?' || c == ')') {
                a->setRow(a->row() + 1);
                if (getOnItem()) {
                    grid[a->row() - 1][a->col()] = getItemUnder();
                    a->setOnItem(false);
                    a->setItemUnder(' ');
                }
                else {
                    grid[a->row() - 1][a->col()] = ' ';
                }
                grid[a->row()][a->col()] = a->showChar();
                a->setOnItem(true);
                a->setItemUnder(c);
                hasMoved = true;
                return;
            }
        }
}

//surroudnings check used only by the goblin, which doesn't care about game objects since it's not actually affecting the grid, only using it as a reference
bool surroundingsCheckForGoblin(char grid[18][70], int row, int col) {
    if (grid[row][col] != '#' && grid[row][col] != 'B' && grid[row][col] != 'S' && grid[row][col] != 'G' && grid[row][col] != 'D') {
        return true;
    }
    
    return false;
}

//array of possible directions to allow for a few less if statements
const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

//the goblin move function and moveHelper function are discussed in detail in my report, so I won't go over them again
void Goblin::move(Goblin* a, char grid[18][70], int row, int col, int &fGobRow, int &fGobCol){
    
    
    int gBestRow = 0;
    int gBestCol = 0;
    
    int minDistance = INT_MAX;
    for (int i = 0; i < 4; ++i) {
        int gobRow = a->row() + directions[i][0];
        int gobCol = a->col() + directions[i][1];
        bool tempGrid[18][70];
        for (int k = 0; k < 18; k++)
            for (int j = 0; j < 70; j++)
                tempGrid[k][j] = false;
        int distance = minDistance;
        if (surroundingsCheckForGoblin(grid, gobRow, gobCol))
            distance = moveHelper(a, grid, tempGrid, gobRow, gobCol, 1);
        if (distance <= getSmellDistance() && distance < minDistance) {
            minDistance = distance;
            gBestRow = gobRow;
            gBestCol = gobCol;
        }
    }
    if (gBestRow != 0 && gBestCol != 0)
        a->setHasMoved(true);
    
    fGobRow = gBestRow;
    fGobCol = gBestCol;
    
    
    
}

int Goblin::moveHelper(Goblin* a, char grid[18][70], bool tempGrid[18][70], int grow, int gcol, int counter) {
    if (grid[grow][gcol] == '@')
        return counter;
    if (counter > a->getSmellDistance())
        return INT_MAX;
    
    tempGrid[grow][gcol] = true;
    
    int minSteps = INT_MAX;
        if (surroundingsCheckForGoblin(grid, grow+1, gcol) && !tempGrid[grow+1][gcol]) {
            minSteps = std::min(minSteps, moveHelper(a, grid, tempGrid, grow+1, gcol, counter+1));
            //return counter;
        }
    if (surroundingsCheckForGoblin(grid, grow, gcol+1) && !tempGrid[grow][gcol+1]) {
        minSteps = std::min(minSteps, moveHelper(a, grid, tempGrid, grow, gcol + 1,  counter+1));
    }
        if (surroundingsCheckForGoblin(grid, grow-1, gcol) && !tempGrid[grow-1][gcol]) {
            minSteps = std::min(minSteps, moveHelper(a, grid, tempGrid, grow-1, gcol, counter+1));
        }
        if (surroundingsCheckForGoblin(grid, grow, gcol-1) && !tempGrid[grow][gcol-1]) {
            minSteps = std::min(minSteps, moveHelper(a, grid, tempGrid, grow, gcol-1, counter+1));
        }
    
    tempGrid[grow][gcol] = false;
    
    
    
    return minSteps;
    

}
