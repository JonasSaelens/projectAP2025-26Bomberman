#pragma once

#include "logic/EntityModel.hpp"

namespace logic {
    /**
 * @brief Represents a wall or obstacle entity in the game world.
 *
 * Can be configured as either indestructible (boundary/solid walls) or destructible
 * (blocks that can be blown up by bomb explosions), handling destruction and event notifications.
 */
    class Wall : public EntityModel {
    public:
        /**
     * @brief Constructs a new Wall instance.
     *
     * @param position The 2D world coordinates for the wall.
     * @param size The bounding box dimensions of the wall.
     * @param destructible Flag indicating whether the wall can be destroyed by explosions.
     */
        Wall(Vector2 position, Vector2 size, bool destructible)
            : EntityModel(position, size), destructible_(destructible) {
        }

        /**
     * @brief Updates the wall's state each frame.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void update(double /*deltaTime*/) override {
        }

        /**
     * @brief Checks whether this wall is destructible.
     * @return True if the wall can be destroyed, false if it is permanent/indestructible.
     */
        bool isDestructible() const { return destructible_; }

        /**
     * @brief Destroys the wall if it is destructible and currently alive.
     *
     * Dispatches a block destruction event and marks the wall entity for removal.
     *
     * @param fromPlayer Flag indicating if the destruction was caused by a player's action.
     */
        void destroy(bool fromPlayer) {
            if (!isDestructible() || !isAlive()) return;
            notify(Event{EventType::BlockDestroyed, getPosition(), Direction::None, fromPlayer});
            kill();
        }

    private:
        bool destructible_; ///< Flag tracking whether the wall can be destroyed by bombs.
    };
} // namespace logic
