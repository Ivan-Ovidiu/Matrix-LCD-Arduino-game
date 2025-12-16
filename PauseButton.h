class PauseButton {
private:
    uint8_t pin;
    unsigned long lastDebounceTime = 0;  //last time the button changed state
    unsigned long debounceDelay = 50;   //debounce time in milliseconds
    bool lastButtonState = HIGH;         // previous reading
    bool buttonState = HIGH;           

public:
    PauseButton(uint8_t pin) : pin(pin) {}

    void initialise() {
        pinMode(pin, INPUT_PULLUP);  
    }

    bool isPressed() {
        bool reading = digitalRead(pin);

        if (reading != lastButtonState) {
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
            buttonState = reading;
        }

        lastButtonState = reading;

        return buttonState == LOW; //when the button is LOW means it is pressed
    }
};
