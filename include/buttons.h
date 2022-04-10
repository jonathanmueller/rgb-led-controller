#pragma once
#include <Arduino.h>

struct ButtonState {
    int pin;
    bool state; /* current state of the button */
    bool wasPressed; /* true if the button has been PRESSED, as opposed to being held */
    bool isBeingHeld; /* true if the button is currently being HELD, as opposed to being PRESSED */
    bool wasBeingHeld; /* true if the button was being HELD in the previous frame */

    int holdDuration = 800; // ms

    ButtonState() : pin(-1) {}
    ButtonState(int pin) : pin(pin) {}

    void update();

private:
    bool previousState;
    bool previousHoldState;
    unsigned int pressedSince;
    unsigned int unpressedSince;
};

extern ButtonState buttonStates[NUM_DIGITAL_PINS];

void buttons_setup();
void buttons_loop();