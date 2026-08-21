#pragma once

#include "logic/Event.hpp"
#include "logic/Subject.hpp"
#include "logic/Vector2.hpp"

namespace logic {
    /**
 * @brief Abstract base class for all game entities in the world.
 *
 * Inherits from Subject to support the observer pattern. Manages spatial properties
 * such as position, size, and hitboxes, as well as lifecycle states like alive/dead
 * and collision intersection checks.
 */
    class EntityModel : public Subject {
    public:
        /**
     * @brief Constructs a new EntityModel instance.
     *
     * @param position The initial 2D world coordinates.
     * @param size The bounding box dimensions of the entity.
     * @param hitboxOffset Offset for fine-tuning collision detection relative to the position.
     */
        EntityModel(Vector2 position, Vector2 size, Vector2 hitboxOffset = {})
            : position_(position), size_(size), hitboxOffset_(hitboxOffset) {
        }

        /**
     * @brief Virtual destructor for the EntityModel.
     */
        ~EntityModel() override = default;

        /**
     * @brief Pure virtual update method called every frame.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        virtual void update(double deltaTime) = 0;

        /**
     * @brief Gets the current world position of the entity.
     * @return The Vector2 position.
     */
        Vector2 getPosition() const { return position_; }
        /**
     * @brief Gets the dimensions (size) of the entity.
     * @return The Vector2 size.
     */
        Vector2 getSize() const { return size_; }
        /**
     * @brief Gets the collision hitbox offset relative to the entity's position.
     * @return The Vector2 hitbox offset.
     */
        Vector2 getHitboxOffset() const { return hitboxOffset_; }
        /**
     * @brief Checks if the entity is currently alive.
     * @return True if alive, false if dead/destroyed.
     */
        bool isAlive() const { return alive_; }

        /**
     * @brief Checks if this entity's hitbox intersects with another entity's hitbox.
     *
     * @param other The other EntityModel to check collision against.
     * @return True if the bounding boxes intersect, false otherwise.
     */
        bool intersects(const EntityModel &other) const {
            const double aLeft = (position_.x + hitboxOffset_.x) - size_.x / 2.0;
            const double aRight = (position_.x + hitboxOffset_.x) + size_.x / 2.0;
            const double aTop = (position_.y + hitboxOffset_.y) - size_.y / 2.0;
            const double aBottom = (position_.y + hitboxOffset_.y) + size_.y / 2.0;

            const double bLeft = (other.position_.x + other.hitboxOffset_.x) - other.size_.x / 2.0;
            const double bRight = (other.position_.x + other.hitboxOffset_.x) + other.size_.x / 2.0;
            const double bTop = (other.position_.y + other.hitboxOffset_.y) - other.size_.y / 2.0;
            const double bBottom = (other.position_.y + other.hitboxOffset_.y) + other.size_.y / 2.0;

            return aLeft < bRight && aRight > bLeft && aTop < bBottom && aBottom > bTop;
        }

        /**
     * @brief Checks if this entity's hitbox intersects with a specific position and size.
     *
     * @param otherPos The target 2D world coordinates.
     * @param otherSize The dimensions of the target bounding box.
     * @return True if they intersect, false otherwise.
     */
        bool intersects(Vector2 otherPos, Vector2 otherSize) const {
            const double aLeft = (position_.x + hitboxOffset_.x) - size_.x / 2.0;
            const double aRight = (position_.x + hitboxOffset_.x) + size_.x / 2.0;
            const double aTop = (position_.y + hitboxOffset_.y) - size_.y / 2.0;
            const double aBottom = (position_.y + hitboxOffset_.y) + size_.y / 2.0;

            const double bLeft = otherPos.x - otherSize.x / 2.0;
            const double bRight = otherPos.x + otherSize.x / 2.0;
            const double bTop = otherPos.y - otherSize.y / 2.0;
            const double bBottom = otherPos.y + otherSize.y / 2.0;

            return aLeft < bRight && aRight > bLeft && aTop < bBottom && aBottom > bTop;
        }

        /**
     * @brief Marks the entity as killed and triggers a death event notification.
     */
        virtual void kill() {
            if (!alive_)
                return;
            alive_ = false;
            notify(Event{EventType::Died, position_});
        }

    protected:
        /**
     * @brief Updates the entity's position and broadcasts a movement event to observers.
     *
     * @param position The new 2D world coordinates.
     * @param facing The direction the entity is facing during the move.
     */
        void setPosition(Vector2 position, Direction facing = Direction::Down) {
            position_ = position;
            notify(Event{EventType::Moved, position_, facing});
        }

    private:
        Vector2 position_; ///< Current world coordinates of the entity.
        Vector2 size_; ///< Dimensions of the entity's bounding box.
        Vector2 hitboxOffset_{0.0, 0.0}; ///< Positional offset for the collision hitbox.
        bool alive_ = true; ///< Flag tracking whether the entity is currently active/alive.
    };
} // namespace logic
