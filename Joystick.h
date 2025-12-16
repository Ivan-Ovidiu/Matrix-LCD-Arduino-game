
class Joystick {
 public:

  bool running;

  int joystickButtonPin;
  int xPin;
  int yPin;

  int maxX;
  int minX;

  int maxY;
  int minY;

  int maxPossibleValue;

  
  int xMinThreshold;
  int xMaxThreshold;

  int yMinThreshold;
  int yMaxThreshold;

  int axisMiddle;

  int xValue ;
  int yValue ;
  int buttonState;
  float rightPercentage;
  float leftPercentage ;
  float upPercentage ;
  float downPercentage ;

  bool lastButtonState = false;    //tracks previous button state
  int lastButtonTime = 0;  
  int debounceDelay = 200; //debounce delay for the joystick button


  Joystick () {
    this->xValue = 0;
    this->yValue = 0;
    this->buttonState = 0;
    this->rightPercentage = 0;
    this->leftPercentage = 0;
    this->upPercentage = 0;
    this->downPercentage = 0;}

  Joystick (int joystickButtonPin = 0,int xPin = A0, int yPin = A1, int maxX = 1010, int minX = 0, int maxY = 1000, 
                int minY = 0, int maxPossibleValue = 1023,int xMinThreshold = 460,int xMaxThreshold = 520,int yMinThreshold = 470,int yMaxThreshold = 540,int axisMiddle = 512) {
    this->xValue = 0;
    this->yValue = 0;
    this->buttonState = 0;
    this->rightPercentage = 0;
    this->leftPercentage = 0;
    this->upPercentage = 0;
    this->downPercentage = 0;
    this->running = true;
  
  this->joystickButtonPin = joystickButtonPin;
  this->xPin = xPin;
  this->yPin = yPin;

  this->maxX = maxX;
  this->minX = minX;

  this->maxY = maxY;
  this->minY = minY;

  this->maxPossibleValue = maxPossibleValue;

  this->xMinThreshold = xMinThreshold;
  this->xMaxThreshold = xMaxThreshold;

  this->yMinThreshold = yMinThreshold;
  this->yMaxThreshold = yMaxThreshold;

  this->axisMiddle = axisMiddle;
  }

  void setxValue(int xValue) { this->xValue = xValue; }
  void setyValue(int yValue) { this->yValue = yValue; }
  void setbuttonState(int buttonState) { this->buttonState = buttonState; }
  void setrightPercentage(float rightPercentage) { this->rightPercentage = rightPercentage; }
  void setleftPercentage(float leftPercentage) { this->leftPercentage = leftPercentage; }
  void setupPercentage(float upPercentage) { this->upPercentage = upPercentage; }
  void setdownPercentage(float downPercentage) { this->downPercentage = downPercentage; }

  void setInput(int value) { pinMode(value, INPUT); }
  void setOutput(int value) { pinMode(value, OUTPUT); }
  void setInputPullup(int value) { pinMode(value, INPUT_PULLUP); }


  //calculates the procentage of both right and left at which the joystick is being moved to
  void checkXDirection() {
  if (xValue >= xMaxThreshold) {
    leftPercentage = 0;
    if (xValue >= maxX)
      xValue = maxPossibleValue;
    rightPercentage = ((float)(xValue - axisMiddle) * 100.0) / (maxPossibleValue - axisMiddle);  //what percentage
  } else if (xValue <= xMinThreshold) {
    rightPercentage = 0;


    leftPercentage = ((float)(axisMiddle - xValue) * 100.0) / axisMiddle;
  } else {
    leftPercentage = 0;
    rightPercentage = 0;
  }
}

//calculates the procentage of both up and down at which the joystick is being moved to
void checkYDirection() {
if (yValue >= yMaxThreshold) {
    downPercentage = 0;
    if (yValue >= maxY)
      yValue = maxPossibleValue;
    upPercentage = ((float)(yValue - axisMiddle) * 100.0) / (maxPossibleValue - axisMiddle);
  } 
  else if (yValue <= yMinThreshold) {
    upPercentage = 0;
    downPercentage = ((float)(axisMiddle - yValue) * 100.0) / axisMiddle;
  } 
  else {
    upPercentage = 0;
    downPercentage = 0;
  }
  }

//debug funnction for the joystick
void printValues() {
  // Serial.print("X: ");
  // Serial.print(xValue);
  // Serial.print(" | Y: ");
  // Serial.println(yValue);
  Serial.print("Right: ");
  Serial.print(rightPercentage);
  Serial.print("% | Left: ");
  Serial.print(leftPercentage);
  Serial.print("% | Up: ");
  Serial.print(upPercentage);



  Serial.print("% | Down: ");
  Serial.print(downPercentage);
  Serial.print("% | Button: ");
  Serial.println(buttonState);
}

  void update() {
    if (running)
    {
    xValue = analogRead(xPin);
    yValue = analogRead(yPin);
    buttonState = !digitalRead(joystickButtonPin);
    checkXDirection();
    checkYDirection();
    }
  }


//           JOYSTICK BUTTON PRESS HANDLING
  bool buttonPress()
  {
    bool pressed = false;
    if (buttonState && !lastButtonState && (millis() - lastButtonTime > debounceDelay)) {
      pressed = true;
      lastButtonTime = millis(); //resets debounce
    }
 
  lastButtonState = buttonState; //resets the button pressing, to detect the transition from unpressed - pressed,for preventing a long pres
  return pressed ;  

  }
 bool buttonLongPress() {
    static unsigned long pressStart = 0; //tracks when the button was first pressed
    static bool lastButtonStateLong = false;
    const unsigned long longPressTime = 2000; //2 seconds

    // Button just pressed
    if (buttonState && !lastButtonStateLong) {
        pressStart = millis(); // start timing
    }

    // Button released before 3 seconds
    if (!buttonState && lastButtonStateLong) {
        pressStart = 0; //reset timer
    }

    lastButtonStateLong = buttonState; //update previous state

    //Check if button is held for 3 seconds
    if (buttonState && (millis() - pressStart >= longPressTime)) {
        pressStart = 0; // reset so it only triggers once
        return true;
    }

    return false;
}

};

