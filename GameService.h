#ifndef GAMESERVICE_H
#define GAMESERVICE_H

#include "Game.h"
#include "Joystick.h"
#include "LCD.h"
#include "Matrix.h" 
#include "PauseButton.h"
#include "BulletService.h"
#include "LDR.h"

#define PAUSE_BUTTON_PIN A2


extern Joystick joystick;
extern Game game;
extern PauseButton pauseButton;
extern BulletService bulleService; 
extern LDR ldr;

//MAIN FUNCTION
void startGame();


#endif
