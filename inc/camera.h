#ifndef CAMERA_H
#define CAMERA_H

#include <genesis.h>

extern u16 currentCameraX;
extern u16 currentCameraY;

typedef struct
{
    u16 baseX, baseY;
    u16 shakeX, shakeY;
} Camera;

/**
 * @brief Main camera update function
 *
 * Handles camera following with dead zone, smooth interpolation,
 * and boundary clamping. Updates background scrolling.
 */
void mainCamera();

#endif // CAMERA_H
