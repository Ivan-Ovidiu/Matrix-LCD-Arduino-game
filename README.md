# Bullet Conquest
An Arduino based dungeon crawler game featuring multiple rooms, enemy bullets, and an light based invincibility mechanic.

## Table of Contents

Overview
Hardware Components
Game Features
Architecture
How to Play
Installation
Game Mechanics

## Overview
Bullet Conquest is a matrix-based dungeon crawler where players navigate through 4 interconnected rooms, dodging and defeating enemy bullets while trying to survive. The game features difficulty levels, a scoring system, persistent high scores, and a unique invincibility mechanic activated by covering a light sensor.

##  Hardware Components
```
Arduino Board (Uno/Mega)               x1
MAX7219 8x8 LED Matrix Module          x1
16x2 LCD Display                       x1
Joystick Module (with button)          x1
Push Button                            x1
LDR (Light Dependent Resistor)         x1
10kΩ Resistor                          x2
100Ω Resistor                          x1
10µF Electrolytic Capacitor            x1
0.1µF Ceramic Capacitor                x1
Breadboard                             x1
Wires              
```

### Pin Configuration
```
LCD Display (16x2):
  RS  → Pin 8
  EN  → Pin 9
  D4  → Pin 2
  D5  → Pin 4
  D6  → Pin 6
  D7  → Pin 7
  VO  → Pin 5 (PWM - brightness control via software)
  VSS → GND
  VDD → 5V
  A   → 5V (backlight anode)
  K   → GND (backlight cathode)

MAX7219 LED Matrix Driver:
  DIN  → Pin 12
  CLK  → Pin 11
  LOAD → Pin 10
  VCC  → 5V
  GND  → GND (connect BOTH GND pins!)

Joystick Module:
  Button → Pin 13
  VRx    → A0 (X-axis)
  VRy    → A1 (Y-axis)
  VCC    → 5V
  GND    → GND

LDR (Light Sensor):
  One leg → A3
  One leg → GND
  A3 → 10kΩ resistor → 5V (pull-up configuration)

Pause Button:
  One leg → PAUSE_BUTTON_PIN (define in header)
  Other leg → GND
  Enable INPUT_PULLUP in code

```
