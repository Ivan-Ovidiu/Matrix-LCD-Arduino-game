#include "Arduino.h"
#include "GameService.h"
#include <EEPROM.h>


Joystick joystick(13, //button pin
A0, //xpin
A1  //ypin
);
Game game;
PauseButton pauseButton(PAUSE_BUTTON_PIN);
BulletService bulleService(&game);  //sending a copy of the game so i dont mess up with the current session of the game
LDR ldr(A3, 100);

// ---------------------------
// Forward declarations
// ---------------------------
void startGame(); 
void resetGameStatics(); // Reset all static flags for new game
          
void stateMachine();
bool saveHighscore(int newScore);
void handleMenu();
void handleDifficultySelection();
void handlePause();

void pauseButtonPressed();
void handleInGame();
void handleExit();
void handleHighscore();
void checkInvincibilityTrigger();

void handleJoystickChoices();
void handleJoystickPressType();
void JoystickPress();

void moveLeft();
void moveRight();
void moveUp();
void moveDown();

void readJoystickMovement();
void movePlayer();
void displayGameStats();
void play(); 

// ---------------------------
// Control variables
// ---------------------------
bool shouldResetGameStatics = false;  // Flag to reset static variables
unsigned long lastMoveTimeChoices = 0;
const unsigned long choiceRepeatDelay = 500;  // ms between choice moves

int leftRightIndex = 0;
int upDownIndex = 0;
static int lastUpDownIndex = -1; // used to redraw menu when changed

//difficulty
static int lastDifficultyIndex = -1;


// press detection
unsigned long lastJoystickAction = 0;
const unsigned long joystickDebounce = 300;
bool longPressed = false;

//Game movement
bool matrixChanged = true;
bool gameStarted = false;
unsigned long lastMoveTime = 0;

//In game redrawing of the stats flag
bool needsRedraw = false;
bool needsRedrawResume = false;

//Pause state
unsigned long lastPulseTime = 0;
bool textVisible = true;


//Exit state
unsigned long lastExitPulse = 0;
bool exitTextVisible = true;

//MAIN GAME FUNCTION
void startGame(){
   // constantly update joystick 
  joystick.update();

  
  checkInvincibilityTrigger();
  game.updateInvincibility();  //checks if 10 seconds have passed


  // Handle menu/game navigation using directional input
  handleJoystickChoices();

  // Handle short/long press behavior
  handleJoystickPressType();

  // Run the state machine (dispatches to appropriate handle functions)
  stateMachine();

  
}



// ---------------------------
// STATE MACHINE
// ---------------------------
void stateMachine() {


  switch (game.state) {
    case MENU:
      handleMenu();
      break;
    case SELECT_DIFFICULTY:
      handleDifficultySelection();
      break;
    case PAUSE:
    handlePause();
      break;

    case IN_GAME:
      handleInGame();
      break;

    case EXIT:
      handleExit();
      break;

    case SHOW_HIGHSCORE:
      handleHighscore();
      break;
    
    default:
      game.state = MENU;
      break;
  }
}

// ---------------------------
// Menu handling
// ---------------------------
void handleMenu() {
  //show menu and only redraw when selection changes
  String menuText[4] = {"Play", "Difficulty", "Highscore", "Exit"};

  if (lastUpDownIndex != upDownIndex) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("> ");
    lcd.print(menuText[upDownIndex]);
    lcd.setCursor(0, 1);
    lcd.print(menuText[(upDownIndex + 1) % 4]);
    lastUpDownIndex = upDownIndex;
  }
}

void handleDifficultySelection() {
  String difficultyText[3] = {"EASY", "MEDIUM", "HARD"};
  
  if (lastDifficultyIndex != upDownIndex) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Difficulty:");
    lcd.setCursor(0, 1);
    lcd.print("> ");
    lcd.print(difficultyText[(upDownIndex+1)%3]);
    lastDifficultyIndex = upDownIndex;
  }
}

void handleExit() {
  unsigned long currentTime = millis();

  if (currentTime - lastExitPulse > 500) {
    lastExitPulse = currentTime;
    exitTextVisible = !exitTextVisible;

    lcd.clear();
    if (exitTextVisible) {
      lcd.setCursor(3, 0);
      lcd.print("Bullet");
      lcd.setCursor(3, 1);
      lcd.print("Conquest");
    } else {

    }
  }
}

// ---------------------------
// Pause handling
// ---------------------------

void handlePause() {
    String pauseText[2] = {"RESUME", "MENU"};

    static int lastleftrightIndex = -1;
    unsigned long currentTime = millis();
    bool render = false;

    
    //check if selection changed
    if (lastleftrightIndex != leftRightIndex) {
        lastleftrightIndex = leftRightIndex;
        render = true;
    }

    //pulse text every 500 ms
    if (currentTime - lastPulseTime > 500) {
        lastPulseTime = currentTime;
        textVisible = !textVisible;
        render = true;
    }

    //only redraw when needed to avoid overlapping text
    if (render) {
        lcd.clear();
        
        // Pulsating PAUSE text
        lcd.setCursor(5, 0);
        if (textVisible) {
            lcd.print("PAUSED");
        } else {
            lcd.print("      "); // Empty space when hidden
        }

        // Menu options
        lcd.setCursor(0, 1);
        if (leftRightIndex == 0) {
            lcd.print(">");
            lcd.print(pauseText[0]);
            lcd.print("<");
            lcd.print("   ");
            lcd.print(pauseText[1]);

        } else {
            lcd.print(" ");
            lcd.print(pauseText[0]);
            lcd.print("   >");
            lcd.print(pauseText[1]);
            lcd.print("<");

        }
    }
}

void pauseButtonPressed(){
 if( pauseButton.isPressed())
    game.state = PAUSE;
}

// ---------------------------
// In game handling
// ---------------------------
void checkInvincibilityTrigger() {
  static bool lastLightState = true;  //Start assuming light
  bool currentLightState = ldr.lightIntensity();
  
  // detect transition from light to dark (threshold reached 0)
  if (lastLightState && !currentLightState && game.state == IN_GAME) {
    game.activateInvincibility();
    Serial.println("Invincibility activated!");
  }
  
  lastLightState = currentLightState;
}

//Handle both the actual game and the game's stats and interface ( displayed on matrix and lcd)
void handleInGame() {
  displayGameStats();//lcd stats / info display
    play(); // starts main game on the matrix
  
}  

// Reset all static variables for a new game
void resetGameStatics() {
  // This will be used to reset flags when transitioning to a new game
  // Note: We can't directly reset static variables from here,
  // so we'll use an external flag that each function checks
}

//display game stats
void displayGameStats(){
  static unsigned long startRoomTimer = 0;
  static bool timerStarted = false;
  static int countdown = 4;
  static int oldPlayerHealth = -1 ;
  static int oldBulletsDefeatedInRoom = -1;
  
  // Reset static variables if flag is set
  if (shouldResetGameStatics) {
    startRoomTimer = 0;
    timerStarted = false;
    countdown = 4;
    oldPlayerHealth = -1;
    oldBulletsDefeatedInRoom = -1;
    shouldResetGameStatics = false;  // Clear flag after resetting
  }
  
  switch(game.inGameState){

   case START_ROOM: {

      if (!timerStarted) {
        startRoomTimer = millis();
        timerStarted = true;
        countdown = 4;
        lcd.clear();
      }
      
      unsigned long elapsed = millis() - startRoomTimer;
      
      //3 sec countdown
      if (elapsed < 4000) {
        int currentCount = 4 - (elapsed / 1000);
        if (currentCount != countdown) {
          countdown = currentCount;
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print(countdown);
        }
      } 
      else {
        lcd.clear();
        timerStarted = false;  //reset for next time
       
        game.inGameState = PLAYING;  
      }
      break;
    }
    
    case PLAYING:{ 

        game.updateInvincibility();
        needsRedraw = false;

        // Check if health or bullets changed
        if(oldPlayerHealth != game.playerHealth || oldPlayerHealth == -1 || 
          oldBulletsDefeatedInRoom == -1 || oldBulletsDefeatedInRoom != game.bulletsDefeatedInRoom) {
          needsRedraw = true;
        }
        
        //check if invincibility state changed
        static bool lastInvincibleState = false;
        static int lastInvincibilityTime = -1;
        int currentInvincibilityTime = game.getRemainingInvincibilityTime();
        
        if (game.isInvincible != lastInvincibleState || 
            currentInvincibilityTime != lastInvincibilityTime) {
          needsRedraw = true;
          lastInvincibleState = game.isInvincible;
          lastInvincibilityTime = currentInvincibilityTime;
        }
        
        if (needsRedraw || needsRedrawResume) {
          needsRedrawResume = false;
          lcd.clear();
          
          //normal display
          lcd.setCursor(0, 0);
          lcd.print("HP ");
          
          for (int i = 0; i < game.playerHealth; i++) {
            lcd.write(byte(0));  //Heart
          }
          
          //display shield icon in top right if invincible
          if (game.isInvincible) {
            lcd.setCursor(15, 0);
            lcd.write(byte(1));  //shield custom character
          }
          
          lcd.setCursor(0, 1);
          lcd.print("Room ");
          lcd.print(game.currentRoom);
          lcd.print(" ");
          lcd.print(game.bulletsDefeatedInRoom);
          lcd.print("/");
          lcd.print(game.bulletsRequiredToFinish);
        }
        
        oldPlayerHealth = game.playerHealth;
        oldBulletsDefeatedInRoom = game.bulletsDefeatedInRoom;
          break;
    }

      case GAME_FINISHED: {

        static bool animationPlayed = false;
          static unsigned long finishDisplayTime = 0;
          static int displayPhase = 0; // 0=score, 1=difficulty, 2=health bonus
          static unsigned long phaseStartTime = 0;
          
          if (!animationPlayed) {
            // Calculate total score across all rooms
            int totalBulletsDefeated = 0;
            for (int room = 0; room < totalRooms; room++) {
              totalBulletsDefeated += game.getBulletsForRoom();
            }
            
            // Calculate final score
            int bulletScore = totalBulletsDefeated * game.getDifficultyMultiplier();
            int healthBonus = game.playerHealth * 100 * game.getDifficultyMultiplier();
            game.currentScore = bulletScore + healthBonus;
            
            // Save to highscores
            saveHighscore(game.currentScore);
            
            // Spiral animation
            const int spiralCoords[][2] = {
              {3, 3}, {3, 4}, {4, 4}, {4, 3}, {4, 2}, {3, 2}, {2, 2}, {2, 3}, {2, 4}, {2, 5},
              {3, 5}, {4, 5}, {5, 5}, {5, 4}, {5, 3}, {5, 2}, {5, 1}, {4, 1}, {3, 1}, {2, 1},
              {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6},
              {6, 6}, {6, 5}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0}, {5, 0}, {4, 0}, {3, 0},
              {2, 0}, {1, 0}, {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7},
              {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {7, 6}, {7, 5}, {7, 4},
              {7, 3}, {7, 2}, {7, 1}, {7, 0}
            };
            
            const int spiralLength = 64;
            
            for (int i = 0; i < 8; i++) {
              game.rooms[game.currentRoom][i] = 0;
            }
            
            for (int i = 0; i < spiralLength; i++) {
              int x = spiralCoords[i][0];
              int y = spiralCoords[i][1];
              game.setLed(x, y);
              displayCurrentRoom();
              delay(30);
            }
            
            for (int flash = 0; flash < 3; flash++) {
              delay(200);
              matrix.clearDisplay(0);
              delay(200);
              displayCurrentRoom();
            }
            
            animationPlayed = true;
            finishDisplayTime = millis();
            phaseStartTime = millis();
            displayPhase = 0;
          }
          
          //cycle through score display phases every 2 seconds
          unsigned long currentTime = millis();
          if (currentTime - phaseStartTime >= 3000) {
            displayPhase = (displayPhase + 1) % 3;
            phaseStartTime = currentTime;
            
            lcd.clear();
            if (displayPhase == 0) {
              // Show final score
              lcd.setCursor(1, 0);
              lcd.print("GAME FINISHED!");
              lcd.setCursor(2, 1);
              lcd.print("Score: ");
              lcd.print(game.currentScore);
            } else if (displayPhase == 1) {
              //Show difficulty
              lcd.setCursor(2, 0);
              lcd.print("Difficulty:");
              lcd.setCursor(4, 1);
              if (game.difficulty == EASY) lcd.print("EASY");
              else if (game.difficulty == MEDIUM) lcd.print("MEDIUM");
              else if (game.difficulty == HARD) lcd.print("HARD");
            } else if (displayPhase == 2) {
              // Show health remaining
              lcd.setCursor(1, 0);
              lcd.print("Health Bonus:");
              lcd.setCursor(3, 1);
              for (int i = 0; i < game.playerHealth; i++) {
                lcd.write(byte(0)); // Heart
              }
              lcd.print(" x");
              lcd.print(game.getDifficultyMultiplier());
            }
          }
          
          //return to menu after 8 seconds total
          if (millis() - finishDisplayTime >= 12000) {
            animationPlayed = false;
            displayPhase = 0;
            
            matrix.clearDisplay(0);
            
            // Reset game
            game.state = MENU;
            game.inGameState = START_ROOM;
            game.currentRoom = 0;
            game.bulletsDefeatedInRoom = 0;
            game.roomCompleted = false;
            game.playerHealth = 3;
            game.xPos = 1;
            game.yPos = 1;
            game.currentScore = 0;
            
            // Reset rooms
            game.rooms[0][0] = B11111111;
            game.rooms[0][1] = B10000001;
            game.rooms[0][2] = B10000001;
            game.rooms[0][3] = B10000001;
            game.rooms[0][4] = B10000001;
            game.rooms[0][5] = B10000001;
            game.rooms[0][6] = B10000001;
            game.rooms[0][7] = B11111111;
            
            game.rooms[1][0] = B11111111;
            game.rooms[1][1] = B10000001;
            game.rooms[1][2] = B10100101;
            game.rooms[1][3] = B10000001;
            game.rooms[1][4] = B10000001;
            game.rooms[1][5] = B10100101;
            game.rooms[1][6] = B10000001;
            game.rooms[1][7] = B11111111;
            
            game.rooms[2][0] = B11111111;
            game.rooms[2][1] = B10000001;
            game.rooms[2][2] = B10000001;
            game.rooms[2][3] = B10011001;
            game.rooms[2][4] = B10011001;
            game.rooms[2][5] = B10000001;
            game.rooms[2][6] = B10000001;
            game.rooms[2][7] = B11111111;
            
            game.rooms[3][0] = B11111111;
            game.rooms[3][1] = B10000001;
            game.rooms[3][2] = B10000001;
            game.rooms[3][3] = B10000001;
            game.rooms[3][4] = B10000001;
            game.rooms[3][5] = B10000001;
            game.rooms[3][6] = B10000001;
            game.rooms[3][7] = B11111111;
            
            upDownIndex = 0;
            lastUpDownIndex = -1;
            gameStarted = false;
          }
          break;
        }


    default:
      break;
  }

}


// game loop logic (matrix updates and player movement)
void play() {
  
  // Don't run game logic if game is finished
  if (game.inGameState == GAME_FINISHED) {
    return;
  }
  
  if (!gameStarted) {   //beggining of the game setup
     displayCurrentRoom();
    game.setLed(game.xPos, game.yPos);
    bulleService.initialize();
    game.bulletsDefeatedInRoom = 0;  // Reset bullet counter
    game.roomCompleted = false;
   
  }
  
    //set difficulty based values
   // game.bulletsRequiredToFinish = game.getBulletsForRoom();
    for (int i = 0; i < MAX_BULLETS; i++) {
      bulleService.bullets[i].setMoveSpeed(game.getBulletSpeed());

   matrixChanged = true;
   gameStarted = true;    
}

  pauseButtonPressed();  //if the pause button is pressed pause the game and display pause menu on lcd

 // Check if room is completed
  if (game.bulletsDefeatedInRoom >= game.bulletsRequiredToFinish && !game.roomCompleted) {
    game.roomCompleted = true;
    game.inGameState = FINISH_ROOM;
    // Open exit doors (top and right positions)
    if (game.currentRoom == 0){
    game.clearLed(3, 0);
    game.clearLed(4, 0);
    game.clearLed(0, 3);
    game.clearLed(0, 4);
    }
    else if (game.currentRoom == 1) {
        // Room 1: Open top and right exits (2 dots each = 4 total)
        game.clearLed(0, 3);
        game.clearLed(0, 4);
        game.clearLed(3, 7);
        game.clearLed(4, 7);
    }
    else if (game.currentRoom == 2) {
        // Room 2: Open right and bottom exits (2 dots each = 4 total)
        game.clearLed(3, 7);
        game.clearLed(4, 7);
        game.clearLed(7, 3);
        game.clearLed(7, 4);
    }
    else if (game.currentRoom == 3) {
        // Room 3: Open bottom and left exits (2 dots each = 4 total)
        game.clearLed(7, 3);
        game.clearLed(7, 4);
        game.clearLed(3, 0);
        game.clearLed(4, 0);
    }
    matrixChanged = true;
  }

if (!game.roomCompleted) {
  unsigned long currentTime = millis();
  if (bulleService.shouldSpawn(currentTime)) {
    bulleService.spawnBullet();
    bulleService.updateSpawnTime(currentTime);
  }
  bulleService.updateBullets(matrixChanged); //deleting ongoing bullets!!
}

  bulleService.updateBullets(matrixChanged);

  if (matrixChanged) {
    displayCurrentRoom();
    matrixChanged = false;
  }

  //movement handling
  movePlayer();
}

// ---------------------------
// IN_GAME joystick movement
// ---------------------------
void movePlayer() {
  if (millis() - lastMoveTime < game.playerSpeed) return;

  bool moved = false;
  int oldX = game.xPos;
  int oldY = game.yPos;
  int newX = game.xPos;
  int newY = game.yPos;

  // Calculate new position based on joystick input
  if (joystick.downPercentage > 55) {
    if (game.xPos > 0) {
      newX = game.xPos - 1;
    }
  }
  else if (joystick.upPercentage > 55) {
    if (game.xPos < 7) {
      newX = game.xPos + 1;
    }
  }
  else if (joystick.leftPercentage > 55) {
    if (game.yPos > 0) {
      newY = game.yPos - 1;
    }
  }
  else if (joystick.rightPercentage > 55) {
    if (game.yPos < 7) {
      newY = game.yPos + 1;
    }
  }

  
  if (newX != game.xPos || newY != game.yPos) {
    //check if the bit at position (newX, newY) is 0 so the player can move there
    if ((game.rooms[game.currentRoom][newX] & (1 << newY)) == 0) {
      //position is clear, allow movement
      game.xPos = newX;
      game.yPos = newY;
      moved = true;
    }
  }
  
  if (moved) {
    // Check if player reached any exit position (AT the wall)
    if (game.roomCompleted && 
        ((game.xPos == 0 && (game.yPos == 3 || game.yPos == 4)) ||  // Top wall exits
         (game.xPos == 3 && (game.yPos == 0 || game.yPos == 7)) ||  // Left/Right wall exits
         (game.xPos == 4 && (game.yPos == 0 || game.yPos == 7)) ||  // Left/Right wall exits
         (game.xPos == 7 && (game.yPos == 3 || game.yPos == 4)))) { // Bottom wall exits
      
      // Teleport to next room
      game.clearLed(oldX, oldY);
      
      // Move to next room
      if (game.currentRoom < totalRooms - 1) {
        game.currentRoom++;
        game.xPos = 6;  // Start at center of new room
        game.yPos = 6;
        game.bulletsDefeatedInRoom = 0;
        game.roomCompleted = false;
        game.inGameState = START_ROOM;
        gameStarted = false;  // Trigger room initialization
      }
      else {
        //finished all rooms
         game.inGameState = GAME_FINISHED;
      }
      
      matrixChanged = true;
    } else {
      game.clearLed(oldX, oldY);
      game.setLed(game.xPos, game.yPos);
      matrixChanged = true;
    }
    
    lastMoveTime = millis();

    Serial.print("X: ");
    Serial.print(game.xPos);
    Serial.print("  Y: ");
    Serial.println(game.yPos);
  }
}


// ---------------------------
// Joystick directional handling (menu/game navigation)
// ---------------------------
void handleJoystickChoices() {
  
  if (joystick.leftPercentage > 55) {
    moveLeft();
  } 
  else if (joystick.rightPercentage > 55) {
    moveRight();
  }
  else if (joystick.upPercentage > 55) {
    moveUp();
  }
  else if (joystick.downPercentage > 55) {
    moveDown();
  }
}


//using lastMoveTimeChoices for debouncing to avoid fast repeats
void moveLeft() {
  if (millis() - lastMoveTimeChoices < choiceRepeatDelay) return;
  lastMoveTimeChoices = millis();

  if (game.state == MENU) {
    leftRightIndex = (leftRightIndex + 1) % 4;
  } else if (game.state == PAUSE) {
    leftRightIndex = (leftRightIndex + 1) % 2;
  }
}

void moveRight() {
  if (millis() - lastMoveTimeChoices < choiceRepeatDelay) return;
  lastMoveTimeChoices = millis();

  if (game.state == MENU) {
    // move right is equal to moving left twice in a 3 item menu
    leftRightIndex = (leftRightIndex + 3) % 4;
  } else if (game.state == PAUSE) {
    leftRightIndex = (leftRightIndex + 1) % 2;
  }
}

void moveUp() {
  if (millis() - lastMoveTimeChoices < choiceRepeatDelay) return;
  lastMoveTimeChoices = millis();

  if (game.state == MENU) {
    upDownIndex = (upDownIndex + 1) % 4;
  }
  else if (game.state == SELECT_DIFFICULTY) {
    upDownIndex = (upDownIndex + 1) % 3;
  }
}

void moveDown() {
  if (millis() - lastMoveTimeChoices < choiceRepeatDelay) return;
  lastMoveTimeChoices = millis();

  if (game.state == MENU) {
    upDownIndex = (upDownIndex + 3) % 4;
  }
  else if (game.state == SELECT_DIFFICULTY) {
    upDownIndex = (upDownIndex + 2) % 3;
  }
}

// ---------------------------
// Button press handling (short or long)
// ---------------------------
void JoystickPress() {
  switch (game.state) {
    case MENU:
        lcd.clear();
        if (upDownIndex == 0) {
          game.state = IN_GAME;
          gameStarted = false;
          shouldResetGameStatics = true;  // Reset static flags for new game
        } else if (upDownIndex == 1) {
          game.state = SELECT_DIFFICULTY;
          upDownIndex = 1; // Start at MEDIUM  
        } else if (upDownIndex == 2) {  // Changed from 1 to 2
          game.state = SHOW_HIGHSCORE;
        } else if (upDownIndex == 3) {  // Changed from 2 to 3
          game.state = EXIT;
        }
      break;

     case SELECT_DIFFICULTY:
        lcd.clear();
        // Set difficulty based on selection
        if (upDownIndex == 0) {
          game.difficulty = EASY;
        } else if (upDownIndex == 1) {
          game.difficulty = MEDIUM;
        } else if (upDownIndex == 2) {
          game.difficulty = HARD;
        }
        game.state = IN_GAME;
        gameStarted = false;
        shouldResetGameStatics = true;  // Reset static flags for new game
      break;

      case PAUSE:
        if(leftRightIndex == 0){
          game.state = IN_GAME; 
          needsRedrawResume = true;
        } 
        else{
          game.state = MENU;
          upDownIndex = 0;
          lastUpDownIndex = -1;
        } 
      break;

    case SHOW_HIGHSCORE:
    if(longPressed)
    {
      game.state = MENU;
      longPressed = false;  
      upDownIndex = 0;
      lastUpDownIndex = -1;
    }
    break; 

    case IN_GAME:
      break;

    case EXIT:
        game.state = MENU;
        upDownIndex = 0;
        lastUpDownIndex = -1;
      break;

    default:
        game.state = MENU;
      break;
  }
}

void handleJoystickPressType() {
  // check debounce and short/long press
  if (millis() - lastJoystickAction < joystickDebounce) return;

  if (joystick.buttonLongPress()) {
    longPressed = true;
    JoystickPress();
    lastJoystickAction = millis();
  } else if (joystick.buttonPress()) {
    longPressed = false;
    JoystickPress();
    lastJoystickAction = millis();
  }
}

//====================
// HIGHSCORES EEPROM
//===================
// EEPROM addresses for top 3 scores (each int is 2 bytes)
const int HIGHSCORE_ADDR_1 = 0;
const int HIGHSCORE_ADDR_2 = 2;
const int HIGHSCORE_ADDR_3 = 4;
void initialiseHighscores() {
    // if EEPROM has been initialized (first byte = 255 means uninitialized)
    if (EEPROM.read(HIGHSCORE_ADDR_1) == 255 && EEPROM.read(HIGHSCORE_ADDR_1 + 1) == 255) {
        // Set default scores to 0
        EEPROM.put(HIGHSCORE_ADDR_1, 0);
        EEPROM.put(HIGHSCORE_ADDR_2, 0);
        EEPROM.put(HIGHSCORE_ADDR_3, 0);
    }
}
//Get highscores from EEPROM
void getHighscores(int scores[3]) {
    EEPROM.get(HIGHSCORE_ADDR_1, scores[0]);
    EEPROM.get(HIGHSCORE_ADDR_2, scores[1]);
    EEPROM.get(HIGHSCORE_ADDR_3, scores[2]);
}

//save a new score if its in top 3
bool saveHighscore(int newScore) {

    getHighscores(game.top3Highscores); //get the highscores into the highscore vector
    
    //find where the new score fits
    int position = -1;
    for (int i = 0; i < 3; i++) {
        if (newScore > game.top3Highscores[i]) {
            position = i;
            break;
        }
    }
    
    //if score doesnt make top 3, return false
    if (position == -1) return false;
    
      //shift scores down
    for (int i = 2; i > position; i--) {
        game.top3Highscores[i] = game.top3Highscores[i - 1];
    }
    game.top3Highscores[position] = newScore;
    
    //Save back to EEPROM
    EEPROM.put(HIGHSCORE_ADDR_1, game.top3Highscores[0]);
    EEPROM.put(HIGHSCORE_ADDR_2, game.top3Highscores[1]);
    EEPROM.put(HIGHSCORE_ADDR_3, game.top3Highscores[2]);
    
    return true;
}

unsigned long lastHighscoreScroll = 0;
int highscoreDisplayIndex = 0;
bool highscoreInitialized = false;  
void handleHighscore() {
    unsigned long currentTime = millis();
    static int lastDisplayIndex = -1;
    
    //initialize on first entry 
    if (!highscoreInitialized) {
        highscoreInitialized = true;
        lastDisplayIndex = -1;
        highscoreDisplayIndex = 0;
        lastHighscoreScroll = currentTime;
    }
    
    //scroll through scores every 2 seconds
    if (currentTime - lastHighscoreScroll > 2000) {
        lastHighscoreScroll = currentTime;
        highscoreDisplayIndex = (highscoreDisplayIndex + 1) % 3;
    }
    
    if (lastDisplayIndex != highscoreDisplayIndex) {
        lastDisplayIndex = highscoreDisplayIndex;
        
        int scores[3];
        getHighscores(scores);
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("-  HIGHSCORES  -");
        
        lcd.setCursor(0, 1);
        lcd.print(highscoreDisplayIndex + 1);
        lcd.print(". ");
        lcd.print(scores[highscoreDisplayIndex]);
        lcd.print(" pts");
    }
}

void resetHighscores() {
    EEPROM.put(HIGHSCORE_ADDR_1, 0);
    EEPROM.put(HIGHSCORE_ADDR_2, 0);
    EEPROM.put(HIGHSCORE_ADDR_3, 0);
    
    Serial.println("Highscores reset to 0!");
}