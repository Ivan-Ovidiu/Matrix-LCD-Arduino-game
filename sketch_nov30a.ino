#include "GameService.h"


void setup() {
    Serial.begin(9600);
    pauseButton.initialise();
    matrixInitialization();
    lcdInitialization();

}

void loop() {
   startGame();
}
