#include <genesis.h>
#include <camera.h>
#include "entities/player.h"

// These bounds are in screen coordinates relative to the center of the screen
#define CAMERA_BOUNDS_LEFT 152
#define CAMERA_BOUNDS_RIGHT 152
#define CAMERA_BOUNDS_TOP 115
#define CAMERA_BOUNDS_BOTTOM 115
//Viewport
#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION 224
//MAP SIZE
#define MAP_HEIGHT 100 // Placeholder
#define MAP_WIDTH 100  // Placeholder
//PLAYER SPRITE SIZE
#define PLAYER_WIDTH 10  // Placeholder
#define PLAYER_HEIGHT 10 // Placeholder

// Global variables must have their type specified.
u16 currentCameraX = 0;
u16 currentCameraY = 0;


void mainCamera()
{

    // Stop player sprit leaving screen
    if (player.posX < FIX32(0))
        player.posX = FIX32(0);
    else if (player.posX > FIX32(MAP_WIDTH) - PLAYER_WIDTH)
    {
        player.posX = FIX32(MAP_WIDTH) - PLAYER_WIDTH;
    }

    if (player.posY < FIX32(0))
        player.posY = FIX32(0);
    else if (player.posY > FIX32(MAP_HEIGHT) - PLAYER_HEIGHT)
    {
        player.posY = FIX32(MAP_HEIGHT) - PLAYER_HEIGHT;
    }

    // Player position on the map
    s16 playerPositionXOnMap = F32_toInt(player.posX);
    s16 playerPositionYOnMap = F32_toInt(player.posY);

    // Player position on the screen, relative to the camera
    s16 playerPositionXOnScreen = playerPositionXOnMap - currentCameraX;
    s16 playerPositionYOnScreen = playerPositionYOnMap - currentCameraY;

    // Temporary variables for the new camera position
    s16 newCameraXPosition;
    s16 newCameraYPosition;

    // Calculate new camera X position based on boundaries
    if (playerPositionXOnScreen > CAMERA_BOUNDS_RIGHT)
    {
        // Player has moved past the right boundary, shift the camera
        newCameraXPosition = playerPositionXOnMap - CAMERA_BOUNDS_RIGHT;
    }
    else if (playerPositionXOnScreen < (HORIZONTAL_RESOLUTION - CAMERA_BOUNDS_RIGHT))
    {
        // Player has moved past the left boundary, shift the camera
        newCameraXPosition = playerPositionXOnMap - (HORIZONTAL_RESOLUTION - CAMERA_BOUNDS_RIGHT);
    }
    else
    {
        // Player is within the horizontal dead zone
        newCameraXPosition = currentCameraX;
    }

    // Calculate new camera Y position based on boundaries
    if (playerPositionYOnScreen > CAMERA_BOUNDS_BOTTOM)
    {
        // Player has moved past the bottom boundary, shift the camera
        newCameraYPosition = playerPositionYOnMap - CAMERA_BOUNDS_BOTTOM;
    }
    else if (playerPositionYOnScreen < CAMERA_BOUNDS_TOP)
    {
        // Corrected: Used the correct Y variable and map position for calculation
        newCameraYPosition = playerPositionYOnMap - CAMERA_BOUNDS_TOP;
    }
    else
    {
        // Player is within the vertical dead zone
        newCameraYPosition = currentCameraY;
    }

    // Stop camera within bounds
    // Horizontal
    if (newCameraXPosition < 0)
    {
        newCameraXPosition = 0;
    }
    else if (newCameraXPosition > MAP_WIDTH - HORIZONTAL_RESOLUTION)
    {
        newCameraXPosition = MAP_WIDTH - HORIZONTAL_RESOLUTION;
    }

    // Vertical
    if (newCameraYPosition < 0)
    {
        newCameraYPosition = 0;
    }
    else if (newCameraYPosition > MAP_HEIGHT - VERTICAL_RESOLUTION)
    {
        newCameraYPosition = MAP_HEIGHT - VERTICAL_RESOLUTION;
    }
    u8 bHScroll;
    u8 bVScroll;
    u8 scrollSpeedModifier = 3;
    // Check if camera position changed
    if ((currentCameraX != newCameraXPosition) || (currentCameraY != newCameraYPosition))
    {
        // Final Step: Update the global camera positions and apply them
        currentCameraX = newCameraXPosition;
        currentCameraY = newCameraYPosition;
        //Interpolation for smooth camera movement
        s16 diffX = newCameraXPosition - currentCameraX;
        s16 diffY = newCameraYPosition - currentCameraY;

        // Apply smooth interpolation (divide by 4 for smooth follow)
        currentCameraX += diffX >> 2;
        currentCameraY += diffY >> 2;

        bHScroll = (0 - currentCameraX) >> scrollSpeedModifier;
        bVScroll = currentCameraY;

        // bVScroll is u8, so it's always >= 0
        if(bVScroll > 32) bVScroll = 0;
        /*TO-DO MAP_scrollTo(,newCameraXPosition, newCameraYPosition) */
        //Scrolling Background
        VDP_setHorizontalScroll(BG_B,bHScroll);
        VDP_setVerticalScroll(BG_B, bVScroll);
    }

    // TO-DO Update player sprite position SPR_setPosition(playerSprite, F32_toInt(player.posX)-newCameraPositionX, F32_toInt(player.posY)-newCameraPositionY);
}