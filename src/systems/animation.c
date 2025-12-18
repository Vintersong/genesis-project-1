#include <genesis.h>
#include "systems/animation.h"

void animInit(AnimState* anim, const AnimStateDef* def) {
    if (!anim || !def) return;

    // Cast away const for assignment (frames won't be modified)
    anim->frames = (u16*)def->frames;
    anim->frameCount = def->frameCount;
    anim->currentFrame = 0;
    anim->speed = def->speed;
    anim->timer = 0;
    anim->loop = def->loop;
    anim->finished = FALSE;
}

u16 animUpdate(AnimState* anim) {
    if (!anim || !anim->frames) return 0;

    // Don't update if animation is finished and not looping
    if (anim->finished && !anim->loop) {
        return anim->frames[anim->currentFrame];
    }

    // Increment timer
    anim->timer++;

    // Check if it's time to advance frame
    if (anim->timer >= anim->speed) {
        anim->timer = 0;
        anim->currentFrame++;

        // Check if we've reached the end
        if (anim->currentFrame >= anim->frameCount) {
            if (anim->loop) {
                // Loop back to start
                anim->currentFrame = 0;
            } else {
                // Stop at last frame
                anim->currentFrame = anim->frameCount - 1;
                anim->finished = TRUE;
            }
        }
    }

    return anim->frames[anim->currentFrame];
}

void animReset(AnimState* anim) {
    if (!anim) return;

    anim->currentFrame = 0;
    anim->timer = 0;
    anim->finished = FALSE;
}

void animSetFrame(AnimState* anim, u16 frame) {
    if (!anim) return;

    if (frame < anim->frameCount) {
        anim->currentFrame = frame;
        anim->timer = 0;
        anim->finished = FALSE;
    }
}

bool animIsFinished(const AnimState* anim) {
    if (!anim) return TRUE;
    return anim->finished;
}

void animSetSpeed(AnimState* anim, u16 speed) {
    if (!anim) return;
    anim->speed = speed;
}
