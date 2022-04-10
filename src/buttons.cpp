#include "buttons.h"
#include "led.h"

ButtonState buttonStates[NUM_DIGITAL_PINS];

void buttons_setup() {
    for (uint8_t i = 0; i < LEDGroups.size(); i++) {
        pinMode(LEDGroups[i].button, INPUT_PULLUP);
        buttonStates[LEDGroups[i].button] = ButtonState(LEDGroups[i].button);
    }
    buttons_loop();
}

void buttons_loop() {
    for (uint8_t i = 0; i < LEDGroups.size(); i++) {
        buttonStates[LEDGroups[i].button].update();

        if (buttonStates[LEDGroups[i].button].state) {
            if (getApp().name != "ledgroups") { setApp("ledgroups"); }
        }
    }
}

void ButtonState::update() {
    /* Update "previous" states */
    previousState = state;
    wasBeingHeld = isBeingHeld;

    bool newState = !digitalRead(pin);
    unsigned int currentTime = millis();
    state = newState;

    if (state != previousState) {
        if (state) {
            pressedSince = currentTime;
        } else {
            unpressedSince = currentTime;
        }
    }

    wasPressed = previousState && !state && (pressedSince > currentTime - holdDuration);
    isBeingHeld = state && pressedSince < currentTime - holdDuration;
}
