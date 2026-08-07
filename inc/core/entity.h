#ifndef ENTITY_H
#define ENTITY_H

#include <genesis.h>
#include "systems/collision.h"

typedef struct {
    fix32 posX, posY;
    fix32 velX, velY;
    u16 health, maxHealth;
    u8 currentState;   // holds a per-type state enum value (e.g. PlayerState), cast at use sites
    bool onGround;
    bool facingRight;
    CollisionBox hitbox;
    Sprite* sprite;
} Entity;

/**
 * @brief Apply gravity, friction, ground check, and velocity integration to an entity
 * @param e Pointer to the entity to update
 */
void entityApplyPhysics(Entity* e);

/**
 * @brief Reduce an entity's health by amount, clamped at 0
 * @param e Pointer to the entity taking damage
 * @param amount Damage amount
 * @return TRUE if health reached 0, FALSE otherwise
 */
bool entityTakeDamage(Entity* e, u16 amount);

/**
 * @brief Clamp an entity's horizontal position to screen bounds
 * @param e Pointer to the entity to clamp
 */
void entityKeepInBounds(Entity* e);

#endif // ENTITY_H
