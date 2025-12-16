#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "Player.h"
#include "Bullet.h"
#include <Arduino.h>

#define roomSize 8
#define totalRooms  4

enum States {
    MENU,
    IN_GAME,
    PAUSE,
    EXIT,
    SHOW_HIGHSCORE,
    RESUME,
    SELECT_DIFFICULTY
    
  };   

enum InGameStates{
    START_ROOM,
    PLAYING,
    FINISH_ROOM,
    ATTACK,
    GAME_FINISHED
};
enum Difficulty {
    EASY,
    MEDIUM,
    HARD
};

class Game : public Player, public Bullet {
public:

  States state ;
  InGameStates inGameState;
  Difficulty difficulty;

//rooms memorised in bytes for memory save
  uint8_t currentRoom;
  uint8_t rooms[totalRooms][roomSize];

  int bulletsDefeatedInRoom;
  int bulletsRequiredToFinish;
  bool roomCompleted;
  int top3Highscores[3];

  // Invincibility variables
  bool isInvincible;
  unsigned long invincibilityStartTime;
  unsigned long invincibilityDuration;

  // Score tracking
  int currentScore;

  // game constructor
  Game() : currentRoom(3),state(EXIT),inGameState(START_ROOM) ,
          bulletsDefeatedInRoom(0), bulletsRequiredToFinish(1), roomCompleted(false),
          isInvincible(false), invincibilityStartTime(0), invincibilityDuration(10000),
           difficulty(MEDIUM), currentScore(0) {
    //Room 0
    rooms[0][0] = B11111111;
    rooms[0][1] = B10000001;
    rooms[0][2] = B10000001;
    rooms[0][3] = B10000001;
    rooms[0][4] = B10000001;
    rooms[0][5] = B10000001;
    rooms[0][6] = B10000001;
    rooms[0][7] = B11111111;
    
    // Room 1
    rooms[1][0] = B11111111;
    rooms[1][1] = B10000001;
    rooms[1][2] = B10100101;
    rooms[1][3] = B10000001;
    rooms[1][4] = B10000001;
    rooms[1][5] = B10100101;
    rooms[1][6] = B10000001;
    rooms[1][7] = B11111111;
    
    // Room 2
    rooms[2][0] = B11111111;
    rooms[2][1] = B10000001;
    rooms[2][2] = B10000001;
    rooms[2][3] = B10011001;
    rooms[2][4] = B10011001;
    rooms[2][5] = B10000001;
    rooms[2][6] = B10000001;
    rooms[2][7] = B11111111;
    
    // Room 3
    rooms[3][0] = B11111111;
    rooms[3][1] = B10000001;
    rooms[3][2] = B10000001;
    rooms[3][3] = B10000001;
    rooms[3][4] = B10000001;
    rooms[3][5] = B10000001;
    rooms[3][6] = B10000001;
    rooms[3][7] = B11111111;
    
  }

  //set led at a certain coordonate
  void setLed(uint8_t x, uint8_t y) {
    if(x < roomSize && y < roomSize) {
      rooms[currentRoom][x] |= (1 << y); // "OR" operator for keeping both the old 1 bits and the new one that is being added
    }
  }

  //clear a led at a certain coordonate
  void clearLed(uint8_t x, uint8_t y) {
    if(x < roomSize && y < roomSize) {
      rooms[currentRoom][x] &= ~(1 << y);   // "AND NOT" operator flips all the bits for deleting the chosen led, then applies AND which keeps only the pre existing 1 bits
    }
  }


  
  void changeRoom(uint8_t newRoom) {
    if(newRoom < totalRooms) {
      clearLed(xPos, yPos);  //clear player from old room
      currentRoom = newRoom;
      setLed(xPos, yPos);    //place player in new room
    }
  }


  //activate invincibility
  void activateInvincibility() {
    isInvincible = true;
    invincibilityStartTime = millis();
  }
  
  //check if invincibility is still active
  void updateInvincibility() {
    if (isInvincible && (millis() - invincibilityStartTime >= invincibilityDuration)) {
      isInvincible = false;
    }
  }
  
  //Get remaining invincibility time in seconds
  int getRemainingInvincibilityTime() {
    if (!isInvincible) return 0;
    unsigned long elapsed = millis() - invincibilityStartTime;
    if (elapsed >= invincibilityDuration) return 0;
    return (invincibilityDuration - elapsed) / 1000;
  }


//================
// GAME Difficulty
//================

  // Get bullets required for current room based on difficulty
  int getBulletsForRoom() {
    int baseCount = currentRoom + 1; 
    
    switch(difficulty) {
      case EASY:
        return baseCount * 2; // 2, 4, 6, 8
      case MEDIUM:
        return baseCount * 3; // 3, 6, 9, 12
      case HARD:
        return baseCount * 4; // 4, 8, 12, 16
      default:
        return baseCount * 3;
    }
  }

  int getBulletSpeed() {
    switch(difficulty) {
      case EASY:
        return 1000; // 1 second per move
      case MEDIUM:
        return 700;  // 0.7 seconds per move
      case HARD:
        return 500;  // 0.5 seconds per move
      default:
        return 700;
    }
  }
   int getDifficultyMultiplier() {
    switch(difficulty) {
      case EASY:
        return 1;
      case MEDIUM:
        return 2;
      case HARD:
        return 3;
      default:
        return 1;
    }
  }
  int calculateFinalScore() {
    int bulletScore = bulletsDefeatedInRoom * getDifficultyMultiplier();
    int healthBonus = playerHealth * 100 * getDifficultyMultiplier();
    return bulletScore + healthBonus;
  }
};

#endif
