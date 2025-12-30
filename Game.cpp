// Game.cpp

#include "Game.h"

// Implement these and other Game member functions you may have added.
void generateRooms(char grid[MAX_ROWS][MAX_COLS], int totalRooms); //prototype for other function reference

Game::Game(int goblinSmellDistance)
{
    levelCounter = 0;
    numActors = 0;
    onItem = false;
    itemUnder = ' ';
    inventoryShown = false;
    newLevel = false;
    droppedAxe = false;
    droppedFangs = false;
    gobSmellDistance = goblinSmellDistance;
    p = nullptr;
    for (int i = 0; i < MAX_ROWS; i++) //initialize grid
        for (int j = 0; j < MAX_COLS; j++)
            grid[i][j] = '#';
    for (int i = 0; i < MAX_MONSTERS; i++) //ensure every pointer begins as NULL
        actors[i] = nullptr;
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) { //initialize dropped object pairs to 0, 0, since no object should ever be at 0, 0
        axeCoords[i] = make_pair(0, 0);
        fangCoords[i] = make_pair(0, 0);
        scrollCoords[i] = make_pair(0, 0);
        
        
    }
    numDroppedAxes = 0;
    numDroppedFangs = 0;
    numDroppedScrolls = 0;
}

Game::~Game() {
    for (int i = 0; i < numActors; i++) {
        delete actors[i];
    }
    delete p;
    
}

void Game::spawnPlayer() { // spawn player using a while loop that generates random row and column integers until they land on a blank character
    int pRow = 0; int pCol = 0;
    while (grid[pRow][pCol] != ' ') {
        pRow = randInt(1, 16);
        pCol = randInt(1, 68);
        p->setCol(pCol);
        p->setRow(pRow);
    }
    
    grid[p->row()][p->col()] = '@';
    clearScreen();
    displayGrid(p->row(), p->col(), "");
}

void Game::spawnMonsters() {
    int numMonsters = randInt(2, 5*(levelCounter+1)+1); //formula given to us
    int i = 0;
    while (i < numMonsters) {
        int chooseMonster = randInt(1, 4);
        switch(chooseMonster) {
            case 1: {
                if (levelCounter >= 2) { //bogeymen should only appear on level 2 or deeper
                    actors[numActors] = new Bogeyman(); //dynamically allocate
                    i++;
                    int mRow = 0; int mCol = 0;
                    while (grid[mRow][mCol] != ' ') { //similar to player's spawn function
                        mRow = randInt(1, 16);
                        mCol = randInt(1, 68);
                    }
                    actors[numActors]->setCol(mCol);
                    actors[numActors]->setRow(mRow);
                    
                    grid[actors[numActors]->row()][actors[numActors]->col()] = 'B';
                    numActors++;
                }
                break;
            }
                //similar strategy employed for other monsters, too
            case 2: {
                actors[numActors] = new Snakewoman();
                i++;
                int mRow2 = 0; int mCol2 = 0;
                while (grid[mRow2][mCol2] != ' ') {
                    mRow2 = randInt(1, 16);
                    mCol2 = randInt(1, 68);
                }
                actors[numActors]->setCol(mCol2);
                actors[numActors]->setRow(mRow2);
                
                grid[actors[numActors]->row()][actors[numActors]->col()] = 'S';
                numActors++;
                break;
            }
            case 3: {
                if (levelCounter >= 3) {
                    actors[numActors] = new Dragon();
                    i++;
                    int mRow2 = 0; int mCol2 = 0;
                    while (grid[mRow2][mCol2] != ' ') {
                        mRow2 = randInt(1, 16);
                        mCol2 = randInt(1, 68);
                    }
                    actors[numActors]->setCol(mCol2);
                    actors[numActors]->setRow(mRow2);
                    
                    grid[actors[numActors]->row()][actors[numActors]->col()] = 'D';
                    numActors++;
                }
                break;
            }
            case 4: {
                actors[numActors] = new Goblin(gobSmellDistance);
                i++;
                int mRow2 = 0; int mCol2 = 0;
                while (grid[mRow2][mCol2] != ' ') {
                    mRow2 = randInt(1, 16);
                    mCol2 = randInt(1, 68);
                }
                actors[numActors]->setCol(mCol2);
                actors[numActors]->setRow(mRow2);
                
                grid[actors[numActors]->row()][actors[numActors]->col()] = 'G';
                numActors++;
                break;
            }
        }
    }
}

void Game::moveMonsters() {
    for (int i = 0; i < numActors; i++) { //loop through every monster
        if (actors[i]->getSleep() > 0) { //if the monster is sleeping, do nothing except for decrementing the sleep time
            actors[i]->setSleep(actors[i]->getSleep() - 1);
            return;
        }
        char c = actors[i]->showChar(); //showChar returns a B for Bogeyman, D for dragon, etc
        switch (c) {
            case 'B': {
                if (abs(actors[i]->row() - p->row()) + abs(actors[i]->col() - p->col()) <= 5 ) //Manhattan distance for bogeyman: since Bogeymen can sniff through walls, the manhattan distance works fine -- it find the distance between two grid coordinates
                    actors[i]->move(actors[i], grid, p->row(), p->col()); //use actors move function
                break;
            }
            case 'S': {
                //manhattan distance for snakewomen, too
                if (abs(actors[i]->row() - p->row()) + abs(actors[i]->col() - p->col()) <= 3 )
                    actors[i]->move(actors[i], grid, p->row(), p->col());
                break;
            }
            case 'G': {
                int newGobRow = 0;
                int newGobCol = 0;
                //use dynamic casting. goblin has it's own move function, so a goblin actor needs to call the function. since actors[i] could be any type of monster, dynamic casting is necessary
                //the function also take a goblin pointer parameter
                dynamic_cast<Goblin*>(actors[i])->move(dynamic_cast<Goblin*>(actors[i]),  grid, p->row(), p->col(), newGobRow, newGobCol);
                if (newGobRow != 0 && grid[newGobRow][newGobCol] != '@') { //newGobRow and newGobCol are passed as references, so they will either come back as 0s, indicating that the goblin has not moved, or as new values indicating the goblin's new position
                    if (actors[i]->getOnItem()) { //if the goblin was on an item
                        grid[actors[i]->row()][actors[i]->col()] = actors[i]->getItemUnder(); //set the grid at the goblin's previous location to the char representing the item it was on
                    }
                    else grid[actors[i]->row()][actors[i]->col()] = ' '; //otherwise just make it a blank again
                        
                    if (grid[newGobRow][newGobCol] == ' ') { //if the goblin is about to step onto a normal space
                        actors[i]->setOnItem(false); //reset to false
                        actors[i]->setItemUnder(' '); //goblin is just on a space
                    }
                    else if (grid[newGobRow][newGobCol] != ' ') { //otherwise
                            actors[i]->setOnItem(true); //the goblin is on an object again
                            actors[i]->setItemUnder(grid[newGobRow][newGobCol]);
                        }
                    //no matter what, set the goblin's row and column to the new row and column, and set the grid to character G
                    actors[i]->setRow(newGobRow);
                    actors[i]->setCol(newGobCol);
                    grid[actors[i]->row()][actors[i]->col()] = 'G';
                }
                //if the goblin isn't going to move, indicate as such (this will become important in the attack call)
                else actors[i]->setHasMoved(false);
                
                break;
            }
            case 'D':
                //dragons can't move, but there's a chance they regain health up to their maxHP
                if (randInt(1, 10) == 1)
                    actors[i]->setHP(actors[i]->getHP() + 1);
                break;
        }
    }
}

//function will return a string indicating the status of attack
string Game::monstersAttack() {
    //every actor should have check attack status
    for (int i = 0; i < numActors; i++) {
        //if the monster is sleeping, do nothing
        if (actors[i]->getSleep() > 0)
            return "";
        //if the monster is in fact next to the player
        if ((actors[i]->row() == p->row() && (actors[i]->col() - 1 == p->col() || actors[i]->col() + 1 == p->col())) || (actors[i]->col() == p->col() && (actors[i]->row() - 1 == p->row() || actors[i]->row() + 1 == p->row()))) {
            //only if the monster hasn't moved can it attack
            if (!actors[i]->getHasMoved()) {
                //call the monster's attack function (which also returns a string). I do it this way to allocate a little more to the monster class, which should really be doing the work of attacking
                string s = actors[i]->attack(actors[i], p);
                //update actionString accordingly, depending on the attack result
                if (s == "hit") {
                    actionString += "\n" + actors[i]->getDescription() + " hits you with " + actors[i]->getWeaponName();
                }
                else if (s == "miss") {
                    actionString += "\n" + actors[i]->getDescription() + " swings and misses. Dumb monster.";
                }
                else if (s == "dead" || s == "hit, sleep")
                    return s;
            }
        }
        //if the monster did move, then now it's turn has passed, so this boolean can be reset to false
        actors[i]->setHasMoved(false);
    }
    return "";
}

//below are all the spawn functions for game objects, the stairs, and the golden idol, following a similar logic to the player spawn function
void Game::spawnScroll() {
    int mRow = 0; int mCol = 0;
    while (grid[mRow][mCol] != ' ') {
        mRow = randInt(1, 16);
        mCol = randInt(1, 68);
    }
    grid[mRow][mCol] = '?';
}
void Game::spawnWeapon() {
    int mRow = 0; int mCol = 0;
    while (grid[mRow][mCol] != ' ') {
        mRow = randInt(1, 16);
        mCol = randInt(1, 68);
    }
    grid[mRow][mCol] = ')';
}

void Game::spawnGameObjects() {
    int numObjects = randInt(2, 3);
    int i = 0;
    while (i < numObjects) {
        //there should be equal chance of spawning for scrolls and weapons... using a random integer from 1 to 10 instead of 1 to 2, I believe, makes for less variation? Or at least, that's the logic in my head... but it shouldn't REALLY matter if its (1, 2) or (1, 10) here
        int chooseGameObject = randInt(1, 10);
        switch(chooseGameObject) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                spawnWeapon();
                i++;
                break;
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                spawnScroll();
                i++;
                break;
        }
    }
}

void Game::spawnStairs() {
    int mRow2 = 0; int mCol2 = 0;
    while (grid[mRow2][mCol2] != ' ') {
        mRow2 = randInt(1, 16);
        mCol2 = randInt(1, 68);
    }
    
    grid[mRow2][mCol2] = '>';
}
void Game::spawnGoldenIdol() {
    int mRow = 0; int mCol = 0;
    while (grid[mRow][mCol] != ' ') {
        mRow = randInt(1, 16);
        mCol = randInt(1, 68);
    }
    
    grid[mRow][mCol] = '&';
}

//this surroundings check takes a row and column coordinate to determine if the grid is passable, not regarding weapons, scrolls, stairs, or the golden idol
bool Game::checkSurroundings(int row, int col) {
    if (grid[row][col] != '#' && grid[row][col] != '@' && grid[row][col] != 'B' && grid[row][col] != 'S' && grid[row][col] != 'G' && grid[row][col] != 'D')
        return true;
    
    return false;
}

//simple bool check for a monster at a grid location
bool Game::checkForMonster(int row, int col) {
    if(grid[row][col] == 'B' || grid[row][col] == 'S' || grid[row][col] == 'G' || grid[row][col] == 'D')
        return true;
    return false;
}

//function to delete a monster that has been killed
void Game::deleteMonster(int pos) {
    delete actors[pos];
    actors[pos] = nullptr;
    for (int i = pos; i < numActors; i++) {
        if (i+1 == numActors) //if i+1 is the total number of actors, then we can't access actors[i+1], so this is the last pointer and we should set this pointer to null, since one actor has been deleted
            actors[i] = nullptr;
        else actors[i] = actors[i+1]; //otherwise, move the pointer up by one
            
    }
    
    numActors--;
}

//this function exists mainly to deal with a dead monster's possibility of dropping an item
void Game::killMonster(int i) {
    //set actionString accordingly
    actionString = "You killed a " + actors[i]->getDescription(); //getDescription returns "goblin" or "snakewoman", etc
    char monType = actors[i]->showChar(); //showChar returns the letter corresponding to the monster
    switch(monType) {
        case 'B':
            if (randInt(1, 10) == 1) {
                droppedAxe = true;
                //if a bogeyman is killed and it drops an axe, update the array of dropped axe coordinates accordingly, making the pair of coordinates
                axeCoords[numDroppedAxes] = make_pair(actors[i]->row(), actors[i]->col());
                numDroppedAxes++;
                grid[actors[i]->row()][actors[i]->col()] = ')'; //set the grid accordingly
            }
            else grid[actors[i]->row()][actors[i]->col()] = ' ';
            break;
            //follow a similar logic for the other monsters. dragons will always drop scrolls.
        case 'S':
            if (randInt(1, 3) == 1) {
                droppedFangs = true;
                fangCoords[numDroppedFangs] = make_pair(actors[i]->row(), actors[i]->col());
                grid[actors[i]->row()][actors[i]->col()] = ')';
                numDroppedFangs++;
            }
            else grid[actors[i]->row()][actors[i]->col()] = ' ';
            break;
        case 'D':
            droppedScroll = true;
            scrollCoords[numDroppedScrolls] = make_pair(actors[i]->row(), actors[i]->col());
            grid[actors[i]->row()][actors[i]->col()] = '?';
            numDroppedScrolls++;
            break;
        case 'G':
            //goblins can drop either axes or fangs
            if (randInt(1, 3) == 1) {
                if (randInt(1, 2) == 1) {
                    droppedAxe = true;
                    axeCoords[numDroppedAxes] = make_pair(actors[i]->row(), actors[i]->col());
                    numDroppedAxes++;
                }
                else {
                    droppedFangs = true;
                    fangCoords[numDroppedFangs] = make_pair(actors[i]->row(), actors[i]->col());
                    grid[actors[i]->row()][actors[i]->col()] = ')';
                    numDroppedFangs++;
                }
                grid[actors[i]->row()][actors[i]->col()] = ')';
            }
            else grid[actors[i]->row()][actors[i]->col()] = ' ';
            break;
    }
    //call to delete a monster here, only after using its pointer in every way we need
    deleteMonster(i);
}


void Game::play()
{
    //very first thing to do is dynamically allocate a new player and generate the level. this is the only time this will happen.
    p = new Player();
    generateNewLevel();
    cout << "Press q to exit game." << endl;
    bool quit = false; //this boolean only serves to indicate when the while loop should be exited (and thus the game ended)
    while (!quit) {
        //if you, the player, are asleep, your sleep should be decremented.
        if (p->getSleep() > 0) {
            actionString = "You are still asleep for " + to_string(p->getSleep() - 1) + " turns";
            p->setSleep(p->getSleep() - 1);
        }
        //enter the player's turn
        switch(getCharacter()) {
            case ARROW_UP:
                //as will be the case in every command, if you're asleep, you can't do anything
                if (p->getSleep() > 0)
                    break;
                //check for a monster in the upwards direction
                if (checkForMonster(p->row() - 1, p->col())) {
                    for (int i = 0; i < numActors; i++) {
                        //find the monster that is located above you, and attack it using the actor attack function
                        if (actors[i]->row() == p->row() -1 && actors[i]->col() == p->col()){
                            string s = p->attack(p, actors[i]);
                            if (s == "kill") {
                                killMonster(i);
                            }
                            else if (s == "hit") {
                                //update actionString accordingly
                                actionString = "You swing your " + p->getWeaponName() + ", hitting the " + actors[i]->getDescription();
                            }
                            else if (s == "hit, sleep") {
                                actionString = "You swing your " + p->getWeaponName() + ", hitting the " + actors[i]->getDescription() + ", and putting it to sleep for " + to_string(actors[i]->getSleep()) + " turns";
                            }
                            else if (s == "miss") {
                                actionString = "You swing your " + p->getWeaponName() + " and miss :( the " + actors[i]->getDescription();
                            }
                        }
                    }
                }
                //otherwise, move accordingly
                else if (checkSurroundings(p->row() - 1, p->col())) {
                    clearScreen();
                    p->decreaseRow();
                    actionString = "";
                    //displayGrid returns the character you are stepping on
                    itemUnder = displayGrid(p->row(), p->col(), "up");
                    if (itemUnder != ' ') {
                        onItem = true;
                    }
                }
                break;
            //exact same process for other directions
            case ARROW_DOWN:
                if (p->getSleep() > 0)
                    break;
                if (checkForMonster(p->row() + 1, p->col())) {
                    for (int i = 0; i < numActors; i++) {
                        if (actors[i]->row() == p->row() +1 && actors[i]->col() == p->col()){
                            string s = p->attack(p, actors[i]);
                            if (s == "kill") {
                                killMonster(i);
                            }
                            else if (s == "hit") {
                                actionString = "You swing your " + p->getWeaponName() + ", hitting the " + actors[i]->getDescription();
                            }
                            else if (s == "hit, sleep") {
                                actionString = "You swing your " + p->getWeaponName() + ", hitting the " + actors[i]->getDescription() + ", and putting it to sleep for " + to_string(actors[i]->getSleep()) + " turns";
                            }
                            else if (s == "miss") {
                                actionString = "You swing your " + p->getWeaponName() + " and miss :( the " + actors[i]->getDescription();
                            }
                        }
                    }
                }
                else if (checkSurroundings(p->row() + 1, p->col())) {
                    clearScreen();
                    p->increaseRow();
                    actionString = "";
                    itemUnder = displayGrid(p->row(), p->col(), "down");
                    if (itemUnder != ' ')
                        onItem = true;
                }
                break;
            case ARROW_LEFT:
                if (p->getSleep() > 0)
                    break;
                if (checkForMonster(p->row(), p->col() -1)) {
                    for (int i = 0; i < numActors; i++) {
                        if (actors[i]->row() == p->row() && actors[i]->col() == p->col() -1){
                            string s = p->attack(p, actors[i]);
                            if (s == "kill") {
                                killMonster(i);
                            }
                            else if (s == "hit") {
                                actionString = "You swing your " + p->getWeaponName() + ", hitting the " + actors[i]->getDescription();
                            }
                            else if (s == "hit, sleep") {
                                actionString = "You swing your " + p->getWeaponName() + ", hitting the " + actors[i]->getDescription() + ", and putting it to sleep for " + to_string(actors[i]->getSleep()) + " turns";
                            }
                            else if (s == "miss") {
                                actionString = "You swing your " + p->getWeaponName() + " and miss :( the " + actors[i]->getDescription();
                            }
                        }
                    }
                }
                else if (checkSurroundings(p->row(), p->col() - 1)) {
                    clearScreen();
                    p->decreaseCol();
                    actionString = "";
                    itemUnder = displayGrid(p->row(), p->col(), "left");
                    if (itemUnder != ' ')
                        onItem = true;
                }
                break;
            case ARROW_RIGHT:
                if (p->getSleep() > 0)
                    break;
                if (checkForMonster(p->row(), p->col() +1)) {
                    for (int i = 0; i < numActors; i++) {
                        if (actors[i]->row() == p->row() && actors[i]->col() == p->col()+1){
                            string s = p->attack(p, actors[i]);
                            if (s == "kill") {
                                killMonster(i);
                            }
                            else if (s == "hit") {
                                actionString = "You swing your " + p->getWeaponName() + ", hitting the " + actors[i]->getDescription();
                            }
                            else if (s == "hit, sleep") {
                                actionString = "You swing your " + p->getWeaponName() + ", hitting the " + actors[i]->getDescription() + ", and putting it to sleep for " + to_string(actors[i]->getSleep()) + " turns";
                            }
                            else if (s == "miss") {
                                actionString = "You swing your " + p->getWeaponName() + " and miss :( the " + actors[i]->getDescription();
                            }
                        }
                    }
                }
                else if (checkSurroundings(p->row(), p->col() + 1)) {
                    clearScreen();
                    p->increaseCol();
                    actionString = "";
                    itemUnder = displayGrid(p->row(), p->col(), "right");
                    if (itemUnder != ' ')
                        onItem = true;
                }
                break;
            case 'i':
                if (p->getSleep() > 0)
                    break;
                //if the inventory is to be displayed, the screen should be cleared and the inventory displayed using a function written in actor.cpp
                clearScreen();
                p->displayInventory();
                inventoryShown = true; //update this bool accordingly
                break;
            case 'w': {
                if (p->getSleep() > 0)
                    break;
                clearScreen();
                p->displayInventory();
                cout << "Type a character corresponding to an inventory item" << endl;
                //after displaying the inventory, the player should type a corresponding character
                char c;
                cin >> c; //read the character
                int i = 0;
                for (char tempChar = 'a'; tempChar < c; tempChar++) //make use of ASCII to increment a temp variable until it reaches c, simultaneously incrementing an integer that will represent the inventory array position
                    i++;
                Weapon* temp = nullptr;
                if (i > 26) { //if i is over 26, the accessWeapons call will fail and generate an out of bounds error
                    actionString = "Please type a character from a-z";
                    break;
                }
                else {
                    temp = p->accessWeapons(i);
                }
                //accessWeapons uses dynamic casting, so if the function returns a null pointer, that means the character typed by the player did not correspond to a weapon
                if (temp != nullptr) {
                    p->setWeapon(temp);
                    actionString = "You are now wielding a " + temp->getDescription();
                }
                else actionString = "You must type a character corresponding to a weapon in your inventory.";
                temp = nullptr;
                break;
            }
            case 'r': {
                if (p->getSleep() > 0)
                    break;
                p->displayInventory();
                char c;
                cin >> c;
                int i = 0;
                for (char tempChar = 'a'; tempChar < c; tempChar++)
                    i++;
                Scroll* temp = nullptr;
                if (i > 26) //same logic as before
                    actionString = "Please type a character from a-z";
                else {
                    temp = p->accessScrolls(i);
                }
                if (temp != nullptr) {
                    //read the scroll's name and update whatever aspect of the game accordingly
                    string s = temp->getDescription();
                    if (s == "Scroll of teleportation") {
                        p->teleport(grid);
                        actionString = "What?! Where are you?!";
                    }
                    else if (s == "Scroll of improve armor") {
                        p->setArmor(p->getArmor() + temp->getImproveAmt()); //each scroll has a member improve amount, which specifies the amount it's corresponding characteristic should be increased by
                        actionString = "Check it out. Shiny new armor.";
                    }
                    else if (s == "Scroll of raise strength") {
                        p->setStrength(p->getStrength() + temp->getImproveAmt());
                        actionString = "Your biceps double in size.";
                    }
                    else if (s == "Scroll of enhance health") {
                        p->setMaxHP(p->getMaxHP() + temp->getImproveAmt());
                        actionString = "Your maximum HP increases slightly.";
                    }
                    else if (s == "Scroll of enhance dexterity") {
                        p->setDex(p->getDex() + temp->getImproveAmt());
                        actionString = "Suddenly, it appears that you can jump onto buildings and dodge even the quickest of goblins.";
                    }
                    
                    p->deleteItemFromInventory(i); //scroll items need to be deleted after being used
                }
                else actionString = "You must type a character corresponding to a scroll in your inventory.";
                temp = nullptr;
                break;
            }
            case '>':
                if (p->getSleep() > 0)
                    break;
                //stairs can only be used if you are on them
                if (itemUnder == '>') {
                    actionString = "";
                    levelCounter++;
                    newLevel = true; //a new level must be generated if the stairs are successfully traversed
                    generateNewLevel();
                }
                else actionString = "You are not on the stairs.";
                break;
            case 'c':
                if (p->getSleep() > 0)
                    break;
                p->setStrength(9);
                p->setMaxHP(50); //setting maxHP, not current HP
                break;
            case 'g':
                if (p->getSleep() > 0)
                    break;
                //command only executed if player is on an item
                if (onItem) {
                    switch(itemUnder) {
                        case '&':
                            cout << "You have won the game!" << endl; //cout since actionString won't update if game is won
                            return;
                        case '>':
                            actionString = "You cannot pick up the stairs. Try typing '>' to traverse the stairs.";
                            break;
                        case ')': {
                            string w;
                            //if there are any dropped weapons on the ground
                            if (droppedAxe) {
                                for (int i = 0; i < numDroppedAxes; i++)
                                    //check coordinates of every dropped axe. If the player is standing on one, then the weapon picked up cannot be randomly generated and must be an axe instead
                                    if (p->row() == axeCoords[i].first && p->col() == axeCoords[i].second)
                                        w = p->pickUpWeapon("axe");
                                numDroppedAxes--;
                                if (numDroppedAxes == 0) //afterwards, there may be no remaining dropped axes, so update accordingly
                                    droppedAxe = false;
                            }
                            //follow the same logic for any dropped fangs
                            else if (droppedFangs) {
                                for (int i = 0; i < numDroppedFangs; i++)
                                    if (p->row() == fangCoords[i].first && p->col() == fangCoords[i].second)
                                        w = p->pickUpWeapon("fangs");
                                numDroppedFangs--;
                                if (numDroppedFangs == 0)
                                    droppedFangs = false;
                            }
                            else {
                                //otherwise, pick up a weapon, which this function will decide randomly (excluding magic axes and fangs, of course)
                                w = p->pickUpWeapon("");
                            }
                            if(w == "full")
                                actionString = "Your inventory is full!";
                            else if(w == "mace")
                                actionString = "You pick up a mace";
                            else if (w == "short sword")
                                actionString = "You pick up a short sword";
                            else if (w == "long sword")
                                actionString = "You pick up a long sword";
                            else if (w == "magic axe")
                                actionString = "You pick up a magic axe";
                            else if (w == "magic fangs")
                                actionString = "You pick up magic fangs";
                            else actionString = "some error occurred";
                            break;
                        }
                        case '?': {
                            string s;
                            //if there are any dropped scrolls, follow the same logic as dropped axes and scrolls
                            if (droppedScroll) {
                                for (int i = 0; i < numDroppedScrolls; i++)
                                    if (p->row() == scrollCoords[i].first && p->col() == scrollCoords[i].second)
                                        s = p->pickUpScroll("drag");
                                numDroppedScrolls--;
                                if (numDroppedScrolls == 0)
                                    droppedScroll = false;
                            }
                            else {
                                s = p->pickUpScroll(""); //similarly, this function will randomly give you a scroll, excluding Scroll of Teleportation
                            }
                            if(s == "full")
                                actionString = "Your inventory is full!";
                            else if (s == "st")
                                actionString = "You pick up a Scroll of Teleportation";
                            else if (s == "sd")
                                actionString = "You pick up a Scroll of Enhance Dexterity";
                            else if (s == "sa")
                                actionString = "You pick up a Scroll of Improve Armor";
                            else if (s == "sh")
                                actionString = "You pick up a Scroll of Enchance Health";
                            else if (s == "ss")
                                actionString = "You pick up a Scroll of Raise Strength";
                            else actionString = "some error occurred";
                            break;
                        }
                    }
                    //as far as right now is concerned, you are no longer on an item, because you have picked it up. this will update again to true in the move functions if you happen to be adjacent to another item that you move onto. but if you do not move and press g again, it should not be treated like you are still on an item.
                    onItem = false;
                }
                break;
            case 'q':
                quit = true;
                break;
        }
        if (!newLevel && !quit) { //only execute this code if it's not a new level or the game has been quit. new levels should not immediately move monsters or deal with their consequences
            moveMonsters(); //moveMonsters function as seen earlier
            string result = monstersAttack(); //monstersAttack function as seen earlier. monsters move first, and only if they haven't moved (because they can't sniff the player, are next to the player, or they are stuck) should they attempt to attack
            if (result == "dead") { //when the monsters attack, the player may be killed.
                cout << endl << "You have been killed. Game over." << endl;
                return;
            }
            else if (result == "hit, sleep") {
                actionString += "\nYou have been hit by magic fangs and put to sleep for " + to_string(p->getSleep()) + " turns";
            }
            //the player has a chance of regenerating HP
            if (randInt(1, 10) == 1)
                p->setHP(p->getHP() + 1);
            clearScreen();
            if (!inventoryShown) //if the inventory is shown, we don't want to overwrite that
                displayGrid();
            else inventoryShown = false; //reset it to false
        }
        newLevel = false; //reset this to false too
        ;
    }
}

char Game::displayGrid(int pRow, int pCol, string dir) {
    char temp = ' ';
    //if the playaer is now on a object (that it's allowed to be on)
    if (grid[pRow][pCol] == ')' || grid[pRow][pCol] == '?' || grid[pRow][pCol] == '&' || grid[pRow][pCol] == '>') {
        temp = grid[pRow][pCol];
    }
    //if player is already on an item, update the grid accordingly so that it correctly displays the item again.
    if (onItem) {
        grid[pRow][pCol] = '@';
        if (dir == "up")
            grid[pRow+1][pCol] = itemUnder;
        else if (dir == "down")
            grid[pRow-1][pCol] = itemUnder;
        else if (dir == "right")
            grid[pRow][pCol-1] = itemUnder;
        else if (dir == "left")
            grid[pRow][pCol+1] = itemUnder;
        
        onItem = false;
        itemUnder = ' ';
    }
    //otherwise, just set the grid to a blank space
    else {
        grid[pRow][pCol] = '@';
        if (dir == "up")
            grid[pRow+1][pCol] = ' ';
        else if (dir == "down")
            grid[pRow-1][pCol] = ' ';
        else if (dir == "right")
            grid[pRow][pCol-1] = ' ';
        else if (dir == "left")
            grid[pRow][pCol+1] = ' ';
    }
    //write out the grid
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++)
            cout << grid[i][j];
        cout << endl;
    }
    //and write out the actionString, starting with player statistics
    cout << "Level: " << levelCounter << ", Hit points: " << p->getHP() << ", Armor: " << p->getArmor() << ", Strength: " << p->getStrength() << ", Dexterity: " << p->getDex() << endl;
    
    cout << endl << actionString << endl;
    
    return temp;
}

//same function but without player row and column (utilized by generatelevel)
void Game::displayGrid() {
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++)
            cout << grid[i][j];
        cout << endl;
    }
    cout << "Level: " << levelCounter << ", Hit points: " << p->getHP() << ", Armor: " << p->getArmor() << ", Strength: " << p->getStrength() << ", Dexterity: " << p->getDex() << endl;
    
    cout << endl << actionString << endl;
    //actionString = "";
}

//struct to represent rooms
struct Room {
    int x, y;       // Top-left corner coordinates of the room
    int width, height;
};


//function that generates a new Level
void Game::generateNewLevel() {
    actionString = ""; //reset actionString, since all info from the previous level is irrelevant now
    
    //reset all actors to nullptr, since old actors are irrelevant
    for (int i = 0; i < numActors; i++) {
        delete actors[i];
        actors[i] = nullptr;
        
    }
    numActors = 0;
    
    //reset all pairs, since they're irrelevant too
    for (int i = 0; i < 5; i++) {
        axeCoords[i] = make_pair(0, 0);
        fangCoords[i] = make_pair(0, 0);
        scrollCoords[i] = make_pair(0, 0);
        
        
    }
    numDroppedAxes = 0;
    numDroppedFangs = 0;
    numDroppedScrolls = 0;
    
    //reset the grid
    for (int i = 0; i < 18; i++)
        for (int j = 0; j < 70; j++)
            grid[i][j] = '#';
    
    int numRooms = randInt(3, 5); //generate a random number of rooms, between 3 and 5, since that's what I found has the best aesthetic while balancing the likelihood of the program stalling from the issue of too much overlap between rooms
    
    generateRooms(grid, numRooms); //generate rooms
    
    //the following for loops are precautionary, and ensure that the grid's border is upheld even after room and hallway generation
    for (int i = 0; i < MAX_ROWS; i++) {
        grid[i][0] = '#';
        grid[i][69] = '#';
    }
    for (int j = 0; j < MAX_COLS; j++) {
        grid[0][j] = '#';
        grid[17][j] = '#';
    }

    
    //call all spawn functions
    spawnPlayer();
    spawnMonsters();
    spawnGameObjects();
    if (levelCounter < 4) //at level 4, there should be no more stairs. this is the final level.
        spawnStairs();
    if (levelCounter == 4) //only on level 4 should the golden idol spawn
        spawnGoldenIdol();
    
    clearScreen();
    displayGrid();

}

// Function to add a room to the grid
void addRoom(char grid[MAX_ROWS][MAX_COLS], const Room& room) {
    for (int i = room.x; i < room.x + room.width; i++) {
        for (int j = room.y; j < room.y + room.height; j++) {
            grid[i][j] = ' ';
        }
    }
}

// Function to check if a room overlaps with existing rooms
bool isOverlap(const Room& room, const Room* rooms, int numRooms) {
    for (int i = 0; i < numRooms; i++) { //cycle through all existing rooms
        const Room& existingRoom = rooms[i];
        //this if statement checks for overlap between two rooms, based on their x and y coordinates + their width/height
        if (room.x <= existingRoom.x + existingRoom.width &&
            room.x + room.width >= existingRoom.x &&
            room.y <= existingRoom.y + existingRoom.height &&
            room.y + room.height >= existingRoom.y) {
            return true;  // overlap found
        }
        //also checking the borders
        if (room.x + room.height >= 16 || room.y + room.width >= 69)
            return true;
    }
    return false;  //no overlap
}

void addHorizontalHallway(char grid[MAX_ROWS][MAX_COLS], int x1, int y1, int y2) {
    //use min and max std functions to ensure that the hallway travels in the correct direction (i.e. to the right and not left if the second room is to the right)
    for (int y = min(y1, y2); y <= max(y1, y2); y++) {
        grid[x1][y] = ' ';
    }
}


void addVerticalHallway(char grid[MAX_ROWS][MAX_COLS], int y, int x1, int x2) {
    for (int x = min(x1, x2); x <= max(x1, x2); x++) {
        grid[x][y] = ' ';
    }
}

//fnction to generate hallways between rooms
void generateHallways(char grid[MAX_ROWS][MAX_COLS], int numRooms, const Room rooms[8]) {
    for (int i = 1; i < numRooms; i++) {
        //grab the centers for each room (or approximate centers)
        int prevCenterX = rooms[i - 1].x + rooms[i - 1].width / 2;
        int prevCenterY = rooms[i - 1].y + rooms[i - 1].height / 2;
        int currCenterX = rooms[i].x + rooms[i].width / 2;
        int currCenterY = rooms[i].y + rooms[i].height / 2;

        //this randInt call allows for slight variation -- hallways could begin with a horizontal or vertical hallway, and use either the previous center or current center as the start
        if (randInt(1, 2) == 1) {
            addHorizontalHallway(grid, prevCenterX, prevCenterY, currCenterY);
            addVerticalHallway(grid, currCenterY, prevCenterX, currCenterX);
        } else {
            addVerticalHallway(grid, prevCenterY, prevCenterX, currCenterX);
            addHorizontalHallway(grid, currCenterX, prevCenterY, currCenterY);
        }
    }
}

void generateRooms(char grid[MAX_ROWS][MAX_COLS], int maxRooms) {
    Room rooms[8];  // Array to store up to 8 rooms
    int numRooms = 0;

    while (numRooms < maxRooms) { //use a while loop to ensure that the total number of rooms randomly generated are all placed onto the grid
        int width = randInt(5, 8);
        int height = randInt(7, 15);
        int x = randInt(1, 17);
        int y = randInt(1, 69);

        Room newRoom = {x, y, width, height};
        

        if (!isOverlap(newRoom, rooms, numRooms)) {
            rooms[numRooms] = newRoom;
            addRoom(grid, newRoom);
            ++numRooms;
        }
    }
    
    generateHallways(grid, numRooms, rooms); //call the generate hallways function
}

//only 937 lines!
