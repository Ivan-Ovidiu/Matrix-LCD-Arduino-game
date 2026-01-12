# Bullet Conquest
An Arduino based dungeon crawler game featuring multiple rooms, enemy bullets, and an light based invincibility mechanic.

## Table of Contents
- [Overview](#overview)
- [Hardware Components](#hardware-components)
- [Game Features](#game-features)
- [Architecture](#architecture)
- [How to Play](#how-to-play)

## Overview
Bullet Conquest is a matrix-based dungeon crawler where players navigate through 4 interconnected rooms, dodging and defeating enemy bullets while trying to survive. The game features difficulty levels, a scoring system, persistent high scores, and a unique invincibility mechanic activated by covering a light sensor.

## Hardware Components
```
Arduino Board (Uno)                    x1
MAX7219 8x8 LED Matrix Module          x1
16x2 LCD Display                       x1
Joystick Module (with button)          x1
Push Button                            x1
LDR (Light Dependent Resistor)         x1
220kΩ Resistor                         x3
10µF Electrolytic Capacitor            x1
0.1µF Ceramic Capacitor                x1
Breadboard                             x2
Wires              
```

### Pin Configuration
```
LCD Display (16x2):
  RS   Pin 8
  EN   Pin 9
  D4   Pin 2
  D5   Pin 4
  D6   Pin 6
  D7   Pin 7
  VO   Pin 5 (PWM - brightness control via software)
  VSS  GND
  VDD  5V
  A    5V (backlight anode)
  K    GND (backlight cathode)

MAX7219 LED Matrix Driver:
  DIN   Pin 12
  CLK   Pin 11
  LOAD  Pin 10
  VCC   5V
  GND   GND (connect BOTH GND pins!)

Joystick Module:
  Button  Pin 13
  VRx     A0 (X-axis)
  VRy     A1 (Y-axis)
  VCC     5V
  GND     GND

LDR (Light Sensor):
  One leg  A3
  One leg  GND
  A3 - 10kΩ resistor - 5V (pull-up configuration)

Pause Button:
  One leg - PAUSE_BUTTON_PIN (define in header)
  Other leg - GND
  Enable INPUT_PULLUP in code

```

## Game Features

### Core Gameplay

4 Unique Rooms: Each with different obstacle layouts
Progressive Difficulty: Complete rooms by defeating required number of bullets
Health System: 3 hearts, lose one per bullet hit (unless invincible)
Room Transitions: Exit through unlocked doors when room objectives are met
Dynamic Bullet Spawning: Enemies spawn at varying rates based on difficulty

- Difficulty Levels
```
Difficulty Bullet_Speed Score_Multiplier Bullets/Room
Easy       Slower         1x              Fewer bullets
Medium     Normal         2x              Standard amount
Hard       Faster         3x              More bullets
```
#### Special Features

- **Invincibility Mode**: Cover the LDR sensor to activate 10 seconds of invincibility

- Visual indicator: Shield icon on LCD
- No damage from bullets
- One-time activation per game


- Persistent High Scores: Top 3 scores saved in EEPROM
- Pause System: Pause mid-game with dedicated button
- Victory Animation: Spiral animation on completion


### Architecture

```
MENU
  ├─> SELECT_DIFFICULTY
  ├─> IN_GAME
  │     ├─> START_ROOM (countdown)
  │     ├─> PLAYING
  │     ├─> FINISH_ROOM
  │     └─> GAME_FINISHED
  ├─> SHOW_HIGHSCORE
  └─> EXIT

PAUSE (accessible from IN_GAME)
  ├─> RESUME → IN_GAME
  └─> MENU

```

#### 1. Game Class
Core game state management and room data.
- Key Properties:
```
- state: Current game state (MENU, IN_GAME, PAUSE, etc.)
- inGameState: Sub-state during gameplay (START_ROOM, PLAYING, FINISH_ROOM, GAME_FINISHED)
- currentRoom: Current room index (0-3)
- playerHealth: Remaining health (max 3)
- xPos, yPos: Player position on 8x8 matrix
- difficulty: Selected difficulty level
- rooms[4][8]: Byte arrays representing room layouts
- isInvincible: Invincibility status
```
- Key Methods:
```
- setLed(x, y): Turn on LED at position
- clearLed(x, y): Turn off LED at position
- activateInvincibility(): Start 10-second invincibility
- updateInvincibility(): Check and update invincibility timer
- getBulletSpeed(): Get bullet movement speed based on difficulty
- getDifficultyMultiplier(): Get score multiplier
```
#### 2. BulletService Class
Manages all bullet spawning, movement, and collision detection.
#### Key functionalities:

- Spawn bullets at random positions
- Update bullet positions each frame
- Detect collisions with player
- Handle bullet defeat (collision with walls)
- Control spawn rate based on difficulty

- Key Methods:
```
cpp
- initialize(): Reset all bullets
- spawnBullet(): Create new bullet at valid position
- updateBullets(matrixChanged): Move all active bullets
- shouldSpawn(currentTime): Check if new bullet should spawn
```
#### 3. Joystick Class
Handles joystick input with percentage-based directional detection.
- Key Properties:
```
cpp
- upPercentage, downPercentage: Vertical input (0-100%)
- leftPercentage, rightPercentage: Horizontal input (0-100%)
- buttonPress(): Detect short press
- buttonLongPress(): Detect long press (2+ seconds)
```
#### 4. LDR (Light Sensor) Class
Monitors ambient light for invincibility trigger.
#### Key Features:

- Calibration on startup
- Threshold-based detection
- Returns boolean light state

#### 5. PauseButton Class
Dedicated pause button handling with debouncing.

### How to Play
#### Main Menu Navigation
- Up/Down: Navigate menu options
- Short Press: Select option
- Play: Start new game
- Difficulty: Change difficulty setting
- Highscore: View top 3 scores
- Exit: Exit to title screen
#### In-Game Controls
- Joystick: Move player (4 directions)
- Pause Button: Pause game
- Cover LDR: Activate invincibility (once per game)
#### Pause Menu
- Left/Right: Toggle between RESUME and MENU
- Short Press: Confirm selection


## Wiring diagram
<img width="2688" height="2280" alt="Matrix Diagram_bb" src="https://github.com/user-attachments/assets/c57cf08b-da04-454d-ba75-27c4eb30a3c6" />


## Picture of the setup
![WhatsApp Image 2025-12-16 at 18 46 04](https://github.com/user-attachments/assets/d1057f2b-aafd-4a0d-855a-02a775693519)

