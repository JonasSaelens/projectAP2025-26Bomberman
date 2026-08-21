#pragma once

#include "logic/Direction.hpp"
#include "logic/Vector2.hpp"

namespace logic {

/**
 * @brief Defines the various categories of events that can occur in the game world.
 */
enum class EventType {
    Moved,
    Spawned,
    Died,
    Won,
    Lost,
    BlockDestroyed,
    PowerUpCollected,
    BombPlaced,
    BombTicked,
    BombExploded,
};

/**
 * @brief Represents a game event dispatched to observers via the Subject/Observer pattern.
 *
 * Contains contextual details about what happened, where it happened,
 * and which entities were involved.
 */
struct Event {
    EventType type;                     ///< The specific category of the event.
    Vector2 position{};                 ///< The 2D world coordinates where the event took place.
    Direction facing = Direction::Down; ///< The facing direction associated with the event (if applicable).
    bool fromPlayer = false;            ///< Flag indicating if the action originated from a player.
    bool killedByPlayer = false;        ///< Flag indicating if a death event was caused by a player.
    bool isPlayer = false;              ///< Flag indicating if the affected entity is a player.
};

} // namespace logic
