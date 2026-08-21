#pragma once

#include <memory>
#include <unordered_set>

#include "logic/Character.hpp"
#include "logic/EntityModel.hpp"

namespace logic {
    /**
 * @brief Represents a bomb entity placed in the game world.
 *
 * Handles the fuse timer, explosion logic, and collision pass-through rules
 * for characters standing on or moving past the bomb.
 */
    class Bomb : public EntityModel {
    public:
        /// The standard duration of the bomb fuse in seconds before detonation.
        static constexpr double kFuseSeconds = 2.0;
        /**
     * @brief Constructs a new Bomb instance.
     *
     * @param position The 2D world coordinates where the bomb is placed.
     * @param size The bounding box dimensions of the bomb.
     * @param radius The explosion radius measured in grid tiles.
     * @param owner A weak reference to the character who planted the bomb.
     */
        Bomb(Vector2 position, Vector2 size, int radius, std::weak_ptr<Character> owner)
            : EntityModel(position, size), radius_(radius), owner_(std::move(owner)) {
        }

        /**
     * @brief Updates the bomb's state each frame.
     *
     * Advances the fuse timer, dispatches tick events, and triggers
     * the explosion once the fuse duration is reached.
     *
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void update(double deltaTime) override {
            if (exploded_)
                return;
            timer_ += deltaTime;
            notify(Event{EventType::BombTicked, getPosition()});
            if (timer_ >= kFuseSeconds) {
                explode();
            }
        }

        /**
     * @brief Gets the explosion radius of the bomb.
     * @return The radius in tiles.
     */
        int getRadius() const { return radius_; }
        /**
     * @brief Checks whether the bomb has already exploded.
     * @return True if the bomb has detonated, false otherwise.
     */
        bool hasExploded() const { return exploded_; }
        /**
     * @brief Retrieves the character that planted the bomb.
     * @return A weak pointer to the owner Character.
     */
        std::weak_ptr<Character> getOwner() const { return owner_; }

        /**
     * @brief Checks if the owner is currently allowed to pass through the bomb tile.
     * @return True if pass-through is enabled for the owner, false otherwise.
     */
        bool canOwnerPassThrough() const { return ownerCanPassThrough_; }
        /**
     * @brief Disables pass-through for the owner (forcing solid collision once they leave the tile).
     */
        void disableOwnerPassThrough() { ownerCanPassThrough_ = false; }
        /**
     * @brief Enables pass-through for the owner.
     */
        void allowOwnerPassThrough() { ownerCanPassThrough_ = true; }

        /**
     * @brief Checks if a specific character is allowed to pass through this bomb.
     *
     * @param character Pointer to the character being checked.
     * @return True if the character is in the pass-through set, false otherwise.
     */
        bool canPassThrough(const Character *character) const {
            return passThroughCharacters_.find(character) != passThroughCharacters_.end();
        }

        /**
     * @brief Grants a specific character temporary pass-through rights over this bomb.
     * @param character Pointer to the character.
     */
        void addPassThroughCharacter(const Character *character) {
            passThroughCharacters_.insert(character);
        }

        /**
     * @brief Revokes pass-through rights for a specific character.
     * @param character Pointer to the character.
     */
        void removePassThroughCharacter(const Character *character) {
            passThroughCharacters_.erase(character);
        }

        /**
     * @brief Triggers the bomb's explosion immediately.
     *
     * Sets the explosion state, dispatches the detonation event, and marks the entity for removal.
     */
        void explode() {
            if (exploded_)
                return;
            exploded_ = true;
            notify(Event{EventType::BombExploded, getPosition()});
            kill();
        }

    private:
        double timer_ = 0.0; ///< Elapsed time on the bomb fuse.
        int radius_; ///< Explosion range in tiles.
        bool exploded_ = false; ///< Flag tracking detonation status.
        std::weak_ptr<Character> owner_; ///< The character who spawned the bomb.
        bool ownerCanPassThrough_ = true; ///< Determines if the planter can walk away before solidifying.
        std::unordered_set<const Character *> passThroughCharacters_;
        ///< Set of characters currently allowed to pass through.
    };
} // namespace logic
