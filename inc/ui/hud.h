#ifndef HUD_H
#define HUD_H

#include <genesis.h>

/**
 * @brief Initialize HUD system
 */
void hudInit();

/**
 * @brief Update HUD display each frame
 */
void hudUpdate();

/**
 * @brief Render HUD to screen
 */
void hudRender();

/**
 * @brief Show/hide HUD
 * @param visible TRUE to show, FALSE to hide
 */
void hudSetVisible(bool visible);

#endif // HUD_H
