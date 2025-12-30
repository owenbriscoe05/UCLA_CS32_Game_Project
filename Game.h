//
//  Game.hpp
//  CS32_Project3v2
//
//  Created by Owen Briscoe on 5/13/24.
//

#ifndef Game_hpp
#define Game_hpp

#include <iostream>
#include "Actor.h"
#include <cmath>
//using namespace std;

const int MAX_ROWS = 18;
const int MAX_COLS = 70;
const int MAX_MONSTERS = 26; //using the equation in the spec
const int MAX_GAME_OBJECTS = 29; //I got this number by adding max monsters which can drop game items + max items randomly spawned initially (26 + 3)

class Game
{
public:
    Game(int goblinSmellDistance);
    ~Game();
    void play();
    void generateNewLevel();
    char displayGrid(int pRow, int pCol, string dir);
    void displayGrid();
    void spawnPlayer();
    void spawnMonsters();
    void spawnGameObjects();
    void spawnWeapon();
    void spawnScroll();
    void spawnStairs();
    void spawnGoldenIdol();
    void moveMonsters();
    string monstersAttack();
    void deleteMonster(int pos);
    bool checkSurroundings(int row, int col);
    bool checkForMonster(int row, int col);
    void killMonster(int i);
private:
    char grid[MAX_ROWS][MAX_COLS];
    Actor* actors[MAX_MONSTERS];
    int numActors;
    Player* p;
    int levelCounter;
    bool onItem;
    bool inventoryShown;
    char itemUnder;
    string actionString;
    bool newLevel;
    bool droppedAxe;
    bool droppedFangs;
    bool droppedScroll;
    pair <int, int> axeCoords[MAX_GAME_OBJECTS];
    pair <int, int> fangCoords[MAX_GAME_OBJECTS];
    pair <int, int> scrollCoords[MAX_GAME_OBJECTS];
    int numDroppedAxes;
    int numDroppedFangs;
    int numDroppedScrolls;
    int gobSmellDistance;
};


#endif /* Game_hpp */
