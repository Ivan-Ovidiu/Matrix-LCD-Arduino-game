#ifndef PLAYER_H
#define PLAYER_H

class Player{
public:

//Player stats
  int playerHealth;
  int playerLevel;
  float playerSpeed;

//Player Position
  int xPos;
  int yPos;

  Player(){

    playerHealth = 3;
    playerLevel = 1;
    playerSpeed = 200;
    xPos = 1;
    yPos = 1;
  }  

  void takeDamage(int damage) {playerHealth = playerHealth - damage;}
  void changeXPosLeft() {xPos--;}
  void changeXPosRight(){ xPos++;}
  void changeYPosUp(){ yPos++;}
  void changeYPosDown() {yPos--;}



};

#endif
