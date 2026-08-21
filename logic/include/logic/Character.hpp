#pragma once

#include "logic/Direction.hpp"
#include "logic/EntityModel.hpp"

namespace logic {
    class World;
    /**
 * @brief Represents a moving character entity in the game (player or bot).
 *
 * Manages movement, facing direction, active bombs, power-up statistics,
 * and lifecycle events such as death and respawning.
 */
    class Character : public EntityModel {
    public:
        static constexpr double kBaseSpeed = 0.5;
        static constexpr int kBaseBombCapacity = 1;
        static constexpr int kBaseBombRadius = 1;

        /**
     * @brief Constructs a new Character instance.
     *
     * @param position The initial 2D world coordinates.
     * @param size The bounding box dimensions.
     * @param hitboxOffset Offset for collision detection relative to the position.
     * @param isPlayer Flag indicating whether this character is controlled by a human player.
     */
        Character(Vector2 position, Vector2 size, Vector2 hitboxOffset = {}, bool isPlayer = false)
            : EntityModel(position, size, hitboxOffset),
              speed_(kBaseSpeed),
              bombCapacity_(kBaseBombCapacity),
              bombRadius_(kBaseBombRadius),
              isPlayer_(isPlayer) {
        }

        /**
     * @brief Moves the character in a specified direction over a given time step.
     *
     * @param direction The direction to move in.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void move(Direction direction, double deltaTime) {
            if (direction == Direction::None) {
                setPosition(getPosition(), Direction::None);
                return;
            }

            facing_ = direction;
            Vector2 delta{0.0, 0.0};
            const double distance = speed_ * deltaTime;
            switch (direction) {
                case Direction::Up:
                    delta = {0.0, -distance};
                    break;
                case Direction::Down:
                    delta = {0.0, distance};
                    break;
                case Direction::Left:
                    delta = {-distance, 0.0};
                    break;
                case Direction::Right:
                    delta = {distance, 0.0};
                    break;
                default:
                    break;
            }
            setPosition(getPosition() + delta, facing_);
        }

        /**
     * @brief Updates the character's state each frame.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void update(double /*deltaTime*/) override {
        }

        /**
     * @brief Gets the direction the character is currently facing.
     * @return The facing Direction.
     */
        Direction getFacing() const { return facing_; }
        /**
     * @brief Gets the maximum number of bombs the character can place simultaneously.
     * @return The bomb capacity limit.
     */
        int getBombCapacity() const { return bombCapacity_; }
        /**
     * @brief Gets the explosion radius modifier for bombs placed by this character.
     * @return The bomb radius value.
     */
        int getBombRadius() const { return bombRadius_; }
        /**
     * @brief Gets the current movement speed of the character.
     * @return The movement speed.
     */
        double getSpeed() const { return speed_; }
        /**
     * @brief Gets the number of bombs currently active in the world placed by this character.
     * @return The active bomb count.
     */
        int getActiveBombs() const { return activeBombs_; }
        /**
     * @brief Checks if this character is controlled by a human player.
     * @return True if it is a player, false if it is a bot.
     */
        bool isPlayer() const { return isPlayer_; }
        /**
     * @brief Checks if the character was killed by a player action.
     * @return True if killed by a player, false otherwise.
     */
        bool isKilledByPlayer() const { return killedByPlayer; }
        /**
     * @brief Marks the character as having been killed by a player.
     */
        void setKilledByPlayer() { killedByPlayer = true; }

        /**
     * @brief Checks if the character is currently allowed to place another bomb.
     * @return True if active bombs are below capacity, false otherwise.
     */
        bool canPlaceBomb() const { return activeBombs_ < bombCapacity_; }
        /**
     * @brief Increments the active bomb counter when a new bomb is placed.
     */
        void onBombPlaced() { ++activeBombs_; }

        /**
     * @brief Decrements the active bomb counter when one of the character's bombs explodes.
     */
        void onBombExploded() {
            if (activeBombs_ > 0)
                --activeBombs_;
        }

        /**
     * @brief Grants an extra bomb capacity increase.
     */
        void grantExtraBomb() { ++bombCapacity_; }
        /**
     * @brief Grants an increased explosion radius for future bombs.
     */
        void grantFireBoost() { ++bombRadius_; }
        /**
     * @brief Grants a movement speed boost.
     */
        void grantSpeedBoost() { speed_ += kBaseSpeed * 0.25; }

        /**
     * @brief Kills the character, firing a death notification event.
     */
        void kill() override {
            if (!isAlive())
                return;
            notify(Event{EventType::Died, getPosition(), Direction::None, true, killedByPlayer, isPlayer_});
            EntityModel::kill();
        }

        /**
     * @brief Emits a custom event to observers.
     * @param event The event to emit.
     */
        void emit(const Event &event) { notify(event); }

    private:
        Direction facing_ = Direction::Down; ///< The current direction the character is facing.
        double speed_; ///< Current movement speed.
        int bombCapacity_; ///< Maximum concurrent active bombs allowed.
        int bombRadius_; ///< Explosion radius of planted bombs.
        int activeBombs_ = 0; ///< Counter tracking currently active bombs in-game.
        bool isPlayer_ = false; ///< Flag identifying if the character is a player.
        bool killedByPlayer = false; ///< Flag indicating if death was caused by a player.
    };
} // namespace logic
