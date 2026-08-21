#pragma once

#include "logic/EntityModel.hpp"

namespace logic {
    /**
 * @brief Defines the structural segments of an explosion flame.
 */
    enum class FlamePart {
        Center,
        Vertical,
        Horizontal,
        EndUp,
        EndDown,
        EndLeft,
        EndRight
    };

    /**
 * @brief Represents an active explosion flame entity in the game world.
 *
 * Manages its own lifetime duration, updates elapsed time each frame,
 * and automatically destroys itself once its lifespan expires.
 */
    class Flame : public EntityModel {
    public:
        /**
     * @brief Constructs a new Flame instance.
     *
     * @param position The 2D world coordinates for the flame segment.
     * @param size The bounding box dimensions of the flame segment.
     * @param lifeTime The total duration the flame remains active in seconds.
     * @param part The specific segment type of the flame.
     */
        Flame(Vector2 position, Vector2 size, double lifeTime, FlamePart part)
            : EntityModel(position, size), lifeTime_(lifeTime), part_(part) {
        }

        /**
     * @brief Updates the flame's lifetime state each frame.
     *
     * Accumulates elapsed time and triggers destruction (`kill()`)
     * once the lifetime threshold is reached.
     *
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void update(double deltaTime) override {
            if (!isAlive())
                return;
            elapsed_ += deltaTime;
            if (elapsed_ >= lifeTime_) {
                kill();
            }
        }

        /**
     * @brief Gets the structural segment type of this flame.
     * @return The FlamePart enumeration value.
     */
        FlamePart getPart() const { return part_; }

    private:
        double lifeTime_; ///< Total duration the flame exists before disappearing.
        double elapsed_ = 0.0; ///< Time elapsed since the flame was spawned.
        FlamePart part_; ///< The specific geometric part/segment of the explosion.
    };
} // namespace logic
