#ifndef INPUT_H
#define INPUT_H

#include <genesis.h>

/**
 * @brief Initialize input system and set up joystick event handler
 */
void inputInit();

/**
 * @brief Read joystick state each frame for continuous input (D-pad)
 */
void inputUpdate();

/**
 * @brief Handle button press/release events
 * @param joy Joystick number (JOY_1, JOY_2)
 * @param changed Buttons that changed state
 * @param state Current state of buttons
 */
void joyEventHandler(u16 joy, u16 changed, u16 state);

#endif // INPUT_H
