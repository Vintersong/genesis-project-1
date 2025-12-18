#include <genesis.h>
#include "systems/input.h"
#include "entities/player.h"

void inputInit() {
    // Initialize joystick system
    JOY_init();

    // Set up event handler for button presses
    JOY_setEventHandler(&playerJoyEvent);
}

void inputUpdate() {
    // Delegate to player input handler
    playerHandleInput();
}

void joyEventHandler(u16 joy, u16 changed, u16 state) {
    // Redirect to player event handler
    playerJoyEvent(joy, changed, state);
}
