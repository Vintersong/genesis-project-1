#include <genesis.h>
#include "ui/hud.h"
#include "entities/player.h"
#include "world/zone.h"

static bool hudVisible = TRUE;
static char healthText[16];
static char staminaText[16];
static char zoneText[32];

// Zone names
static const char* zoneNames[] = {
    "CPU",
    "GPU",
    "RAM",
    "STORAGE",
    "HUB",
    "BIOS",
    "RESERVED"
};

void hudInit() {
    hudVisible = TRUE;
    
    // Clear text plane
    VDP_clearTextArea(0, 0, 40, 28);
}

void hudUpdate() {
    if (!hudVisible) return;
    
    // Update text strings
    sprintf(healthText, "HP:%03d/%03d", player.health, player.maxHealth);
    sprintf(staminaText, "SP:%03d/%03d", player.stamina, player.maxStamina);
    sprintf(zoneText, "ZONE:%s", zoneNames[getCurrentZone()]);
}

void hudRender() {
    if (!hudVisible) return;
    
    // Draw HUD text at top of screen
    VDP_drawText(healthText, 1, 1);
    VDP_drawText(staminaText, 1, 2);
    VDP_drawText(zoneText, 1, 27);
    
    // Draw debug info if needed
    #ifdef DEBUG
    char debugText[32];
    sprintf(debugText, "X:%d Y:%d", fix32ToInt(player.posX), fix32ToInt(player.posY));
    VDP_drawText(debugText, 25, 1);
    
    sprintf(debugText, "STATE:%d", player.currentState);
    VDP_drawText(debugText, 25, 2);
    #endif
}

void hudSetVisible(bool visible) {
    hudVisible = visible;
    
    if (!visible) {
        // Clear HUD area
        VDP_clearTextArea(0, 0, 40, 3);
        VDP_clearTextArea(0, 27, 40, 1);
    }
}
