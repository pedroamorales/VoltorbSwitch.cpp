#include "ofApp.h"

/*
    Author's Note:
        Welcome to Voltorb Switch (not voltorb flip). A totally not rip-off of the casino game
        from Pokemon HeartGold-SoulSilver. This project may seem a lot more daunting than the PA0, but don't worry
        The author is here to help!

        I wrote all the comments with the idea that you at least ran the program first and played a bit with the game. 
        I tried to make it as fun as possible so good luck!

        ps. don't hate the author if this project is too hard, it's fault of the staff, not the author. 
*/

//--------------------------------------------------------------
void ofApp::setup(){
/*
    Alright, this part is basically running at the start of the program. So it only runs 1 time.
    In that time it's basically just allocating the necessary files and data that it will use throughout the program. 
*/

    ost.load("Sounds/music_intro.mp3"); // Loading the ost file to be played later
    ost.setLoop(true); // making sure the song never ends
    pointMult_sfx.load("Sounds/sfx/pointIncreaseSfx.mp3"); // this is the sound file when the tile clicked increments the score
    pointsTallied_sfx.load("Sounds/sfx/pointsTallied.mp3"); // sound file for when a level is cleared and points are added to the 'pointBank'
    levelBeat_sfx.load("Sounds/sfx/levelBeat.mp3"); // this is the sound file for when the level is cleared
    rules.load("Assets/rules.png"); // this is the image for the rules
    points.load("Assets/points.png"); // this is the image for the points
        
    // This for-loop is just to make the loading of the individual frames of animations easier to load instead of having 8 lines of code of each frame
    for (int i = 0; i < 9; i++) { 
        ofImage voltorbImage = ofImage();
        voltorbImage.load("Assets/Animations/explode_frames/explode_" + ofToString(i) + ".png");
        voltorb_explosion.push_back(voltorbImage);
    }
    // same as above but with the animation when you flip a tile
    for (int i = 0; i < 4; i++) { 
        ofImage successImage = ofImage();
        successImage.load("Assets/Animations/success_frames/success_" + ofToString(i) + ".png");
        success_animations.push_back(successImage);
    }
    

    setupLevel();    // This method is one of the most important ones. It basically sets up the current level creating the tiles and grid.


    font.load("pokemon-ds-font.ttf", 40); 
    titleFont.load("Silkscreen-Regular.ttf", 100);
    
    ost.setVolume(0.50); // to not make the song too loud
    ost.play(); // this is to make the song play. 

    currentPoints = 1; // This is the current points. Since they work by being multiplied, then it's better to start with 1 

    pullPointsFromBank(); 

}

//--------------------------------------------------------------
void ofApp::setupLevel() {
    // This will set up the current grid (notice that the grid is just a 5x5 matrix)
    vector<vector<int>> level_1 
    = {
        {0, 3, 1, 3, 2},
        {0, 0, 0, 0, 2},
        {0, 2, 1, 3, 0},
        {0, 0, 1, 3, 0},
        {1, 2, 3, 2, 1}
    };
    vector<vector<int>> level_2 
    = {
        {1, 2, 3, 0, 1},
        {0, 2, 0, 1, 2},
        {2, 3, 1, 2, 3},
        {1, 2, 3, 1, 2},
        {3, 1, 0, 2, 1}
    };
    
    vector<vector<int>> level_3 
    = {
        {1, 2, 3, 1, 1},
        {0, 2, 0, 1, 2},
        {2, 0, 1, 2, 0},
        {1, 2, 3, 1, 2},
        {3, 1, 0, 0, 1}
    };


    levelList.clear();
    gameGrids lvl1 = gameGrids(level_1, success_animations, voltorb_explosion);
    levelList.push_back(lvl1);
    gameGrids lvl2 = gameGrids(level_2, voltorb_explosion, success_animations);
    levelList.push_back(lvl2);
    gameGrids lvl3 = gameGrids(level_3, success_animations, voltorb_explosion);
    levelList.push_back(lvl3);

    // This is to reset all the tiles to be flipped off.
    for (auto& grid : levelList) {
        for (auto& row : grid.tileGrid) {
            for (auto& tile : row) {
                if (tile) {
                    tile->flipOff();
                }
            }
        }
    }

    victory = false;
    gameFinished = false;
    defeat = false;


    // This is to assign the current grid to be drawn, processed and of course, play.
    currentTileGrid = levelList[currentLevel].tileGrid;
    currentInfoTileGrid = levelList[currentLevel].infoTileGrid;
    
        if (currentTileGrid.size() == 0) {
            gameFinished = true;
        }
    
    countTiles();
}

//--------------------------------------------------------------

// The update method for the game. Update + draw methods are the core loop of the game.
void ofApp::update(){
    // If you haven't lost, or haven't won, then keep updating the tiles to keep checking which have been flipped.
    if (!victory && !defeat) {
    for (auto& row : currentTileGrid) {
        for (auto& tile : row) {
            if (tile) {
                tile->update();
            }
        }
    }
    if (checkTimer > 0) {
        checkTimer--;
        if (checkTimer == 0) {
                if (checkDefeat()) {
                    defeat = false;
                }
                if (checkVictory()) {
                    levelBeat_sfx.play();
                    victory = true;
                }
            canPlay = true;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetBackgroundColor(41,165,107,255); // This is the green background color.

    if (showRules && !gameFinished) {
        ofSetColor(ofColor::white);
        int widthRules = ofGetWidth() * 5/16;
        int heightRules = ofGetHeight() * 4/10;
        
        int widthPoints = ofGetWidth() * 5.5/16;
        int heightPoints = ofGetHeight() * 4/10;

        rules.draw(ofGetWidth() * 10.5/16, ofGetHeight() * 1/10, widthRules, heightRules);
        font.drawString(to_string(currentLevel + 1), ofGetWidth() * 13.8/16, ofGetHeight() * 1.6/10);


        points.draw(ofGetWidth() * 10/16, ofGetHeight() * 5.5/10, widthPoints, heightPoints);
        ofSetColor(ofColor::black);
        string pointsStr = to_string(currentPoints);
        string storedPointsStr = to_string(storedPoints);

        // Calculate font size or adjust scaling based on the number of digits
        int maxLength = max(pointsStr.length(), storedPointsStr.length());
        int adjustedFontSize = 85 - (maxLength * 10);

        // Load or scale font size dynamically
        titleFont.load("Silkscreen-Regular.ttf", max(adjustedFontSize, 32)); 

        // Center or adjust the position of the points text
        float xPosition = ofGetWidth() * 13.5/16 - (maxLength * 10);
        titleFont.drawString(pointsStr, xPosition, ofGetHeight() * 6.7/10);
        titleFont.drawString(storedPointsStr, xPosition,  ofGetHeight() * 8.7/10);
        ofSetColor(ofColor::white);
    } 

    //For loop to draw the infoTiles
    for (unsigned int row = 0; row < currentTileGrid.size(); row++) {
        for (unsigned int col = 0; col < currentTileGrid[row].size(); col++) {
            if (currentInfoTileGrid[row][col]) {
                currentInfoTileGrid[row][col]->draw();
            }
        }
    }


    //For loop to draw the gameTiles (it's below the info tiles so the background pipe the infotile has gets drawn behind the gameTile)
    for (unsigned int row = 0; row < currentTileGrid.size(); row++) {
        for (unsigned int col = 0; col < currentTileGrid[row].size(); col++) {
            if (currentTileGrid[row][col]) {
                currentTileGrid[row][col]->draw();
            }
        }
    }

    if (victory) {
        ofSetBackgroundColor(ofColor::green);
        font.drawString("Victory! You've won!", ofGetWidth()* 3/4 - 50, ofGetHeight() * 0.8/10);
        font.drawString("Press space to continue!", ofGetWidth()* 1/4 - 50, ofGetHeight());
    }
    else if (defeat) {
        pullPointsFromBank();
        storePointsInBank();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (victory && key == ' ') {
        victory = false;
        if (currentLevel < levelList.size()) {
           currentLevel++; 
        }
        currentTileGrid.clear();
        currentInfoTileGrid.clear();
        setupLevel();
        storePointsInBank();
        currentPoints = 1;
        pointsTallied_sfx.play();
    }

    if (defeat && key == ' ') {
        defeat = false;
        currentLevel = 0;
        currentPoints = 1; 
        currentTileGrid.clear();
        currentInfoTileGrid.clear();
        setupLevel();
    }

    // This is in case you want to toggle the points and rules off to only look at the grid
    if (key == 'R' || key == 'r') {
        showRules = !showRules;
    }
    //Debbuging key to skip levels
    if(key == 'W' || key == 'w'){
        victory = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    //Just for hovering purposes
    for (unsigned int row = 0; row < currentTileGrid.size(); row++) {
        for (unsigned int col = 0; col < currentTileGrid[row].size(); col++) {
            if (currentTileGrid[row][col]) {
                currentTileGrid[row][row]->isHovering(x, y);
            }
        }
    }

}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    for (unsigned int row = 0; row < currentTileGrid.size(); row++) {
        for (unsigned int col = 0; col < currentTileGrid[row].size(); col++) {
            if (currentTileGrid[row][col]) {
                if(currentTileGrid[col][row]->mouseHovering(x, y) && button == OF_MOUSE_BUTTON_1 && canPlay) {
                    if (!currentTileGrid[row][col]->isFlipped()) {
                        currentTileGrid[row][col]->startFlip();
                        updateTileCount(currentTileGrid[col][row]->getValueType());
                        checkTimer = 20;    //Don't lower this any more. It'll break the game. 
                    }
                }

            }
        }

    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

/*
    Author's note:
        - This method might be a bit complicated but fear not.
        It uses something you won't cover in this class but you don't have to worry about that
*/
void ofApp::countTiles() {
    /*
        Imagine a vector where the index is the tileType, and the value is just to keep count of each tile type
        In other words, let's say there's 3 bombs in the entire grid...
        well the tileValueCounts at index tileType::VOLTORB ( sou tileValueCounts[tileType::VOLTORB]) 
        will be equal to 3.

        This does the same for all 4 tileTypes
    */

    tileValueCounts[tileType::VOLTORB] = 0;
    tileValueCounts[tileType::ONE] = 0;
    tileValueCounts[tileType::TWO] = 0;
    tileValueCounts[tileType::THREE] = 0;

    // This is how the code runs through the tileGrid to count all the tileTypes 
    for(unsigned int i = 0; i < currentTileGrid.size(); i++) {
        for(unsigned int j = 0; j < currentTileGrid[i].size(); j++) {
            if(currentTileGrid[i][j]) {
                tileValueCounts[currentTileGrid[i][j]->getValueType()]++;
            }
        }
    }
}

void ofApp::updateTileCount(tileType type) {
    /*
        Author's note:
            - This method is to make the count of each tileType go down after you press a tile.
    */
    if(tileValueCounts.find(type) != tileValueCounts.end()) {
        if (tileValueCounts[type] > 0) { // to not overshoot into the negatives
            tileValueCounts[type]--;
            if(int(type) > 1) {
                pointMult_sfx.play();
            }
            currentPoints *= (int) type; //multiply the score
        }

    }
}


bool ofApp::checkVictory() {
    // Win when there are no more Two's or Three's in the grid
    if (tileValueCounts[tileType::TWO] == 0 && tileValueCounts[tileType::THREE] == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool ofApp::checkDefeat() {
    for (unsigned int i = 0; i < currentTileGrid.size(); i++) {
        for (unsigned int j = 0; j < currentTileGrid[i].size(); j++) {
            if (currentTileGrid[i][j]) {
                if (currentTileGrid[i][j]->getValue() == tileType::VOLTORB) {
                    if (currentTileGrid[i][j]->isFlipped()) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

