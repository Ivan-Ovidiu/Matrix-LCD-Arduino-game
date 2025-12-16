#include "Matrix.h"
#include "GameService.h"

#define DIN_PIN 12
#define LOAD_PIN 10
#define CLK_PIN 11

LedControl matrix(DIN_PIN,CLK_PIN,LOAD_PIN,1);


void matrixInitialization(){
  matrix.shutdown(0, false);      // Wake up the display
  matrix.setIntensity(0, 8);      // Set brightness (0-15)
  matrix.clearDisplay(0);         // Clear the display
}


void displayCurrentRoom() {
    for(uint8_t row = 0; row < roomSize; row++) {
      matrix.setRow(0, row, game.rooms[game.currentRoom][row]);
    }
  }

void show(){
for (int row = 0; row < roomSize; row++) { 
matrix.setRow(0, row, game.rooms[0][row]); 
}
  
}  