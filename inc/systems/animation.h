#ifndef ANIMATION_H
#define ANIMATION_H

#include <genesis.h>

// Custom animation structure for sprite frame management
// Named with "Anim" prefix to avoid conflict with SGDK's Animation type
typedef struct {
    u16* frames;        // Array of frame indices
    u16 frameCount;     // Number of frames in animation
    u16 currentFrame;   // Current frame index
    u16 speed;          // Frames to wait between animation frames
    u16 timer;          // Current timer value
    bool loop;          // Should animation loop?
    bool finished;      // Has animation finished? (for non-looping)
} AnimState;

// Animation definition structure (for defining animations in ROM)
typedef struct {
    const u16* frames;  // Pointer to frame array in ROM
    u16 frameCount;     // Number of frames
    u16 speed;          // Animation speed
    bool loop;          // Loop flag
} AnimStateDef;

/**
 * @brief Initialize an animation from a definition
 * @param anim Pointer to AnimState struct
 * @param def Pointer to AnimStateDef definition
 */
void animInit(AnimState* anim, const AnimStateDef* def);

/**
 * @brief Update animation timer and advance frame if needed
 * @param anim Pointer to AnimState struct
 * @return Current frame index
 */
u16 animUpdate(AnimState* anim);

/**
 * @brief Reset animation to first frame
 * @param anim Pointer to AnimState struct
 */
void animReset(AnimState* anim);

/**
 * @brief Set animation to a specific frame
 * @param anim Pointer to AnimState struct
 * @param frame Frame index to set
 */
void animSetFrame(AnimState* anim, u16 frame);

/**
 * @brief Check if animation has finished (for non-looping animations)
 * @param anim Pointer to AnimState struct
 * @return TRUE if finished, FALSE otherwise
 */
bool animIsFinished(const AnimState* anim);

/**
 * @brief Change animation speed
 * @param anim Pointer to AnimState struct
 * @param speed New speed value
 */
void animSetSpeed(AnimState* anim, u16 speed);

#endif // ANIMATION_H
