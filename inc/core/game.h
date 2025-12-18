#ifndef GAME_H
#define GAME_H

#include <genesis.h>

typedef enum {
    GAME_STATE_TITLE,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER,
    GAME_STATE_TRANSITION
} GameState;

extern GameState currentGameState;

/**
 * @brief Initialize all game systems
 */
void gameInit();

/**
 * @brief Update current game state
 */
void gameUpdate();

/**
 * @brief Transition between game states
 * @param newState The state to transition to
 */
void gameChangeState(GameState newState);

/**
 * @brief Get current game state
 * @return Current GameState
 */
GameState gameGetState();

#endif // GAME_H
