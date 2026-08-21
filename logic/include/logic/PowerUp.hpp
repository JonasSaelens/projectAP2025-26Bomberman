#pragma once

#include "logic/EntityModel.hpp"

namespace logic {

class Character;

/**
 * @brief Defines the available types of collectible power-ups in the game.
 */
enum class PowerUpType { Fire, ExtraBomb, Skates };

/**
 * @brief Abstract base class representing a collectible power-up item in the world.
 *
 * Manages power-up types, collection events, and defines an interface for applying
 * bonuses to characters who collect them.
 */
class PowerUp : public EntityModel {
public:
    /**
     * @brief Constructs a new PowerUp instance.
     *
     * @param position The 2D world coordinates for the power-up.
     * @param size The bounding box dimensions of the power-up.
     * @param type The specific category/effect of the power-up.
     */
    PowerUp(Vector2 position, Vector2 size, PowerUpType type)
        : EntityModel(position, size), type_(type) {
    }

    /**
     * @brief Updates the power-up's state each frame.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
    void update(double /*deltaTime*/) override {
    }

    /**
     * @brief Gets the type of this power-up.
     * @return The PowerUpType enumeration value.
     */
    PowerUpType getType() const { return type_; }

    /**
     * @brief Applies the power-up's specific bonus effect to a character.
     * @param character Reference to the character receiving the effect.
     */
    virtual void applyTo(Character& character) = 0;

    /**
     * @brief Handles the collection of the power-up by a character.
     *
     * Dispatches a collection event and marks the power-up entity for removal.
     *
     * @param fromPlayer Flag indicating if the collector was a human player.
     */
    void collect(bool fromPlayer) {
        notify(Event{EventType::PowerUpCollected, getPosition(), Direction::Down, fromPlayer});
        kill();
    }

private:
    PowerUpType type_; ///< The category/effect of the power-up.
};

/**
 * @brief A power-up that increases a character's bomb explosion radius.
 */
class FirePowerUp : public PowerUp {
public:
    /**
     * @brief Constructs a new FirePowerUp instance.
     * @param position The 2D world coordinates.
     * @param size The bounding box dimensions.
     */
    explicit FirePowerUp(Vector2 position, Vector2 size)
        : PowerUp(position, size, PowerUpType::Fire) {
    }

    /**
     * @brief Applies a fire boost effect to the given character.
     * @param character Reference to the character.
     */
    void applyTo(Character& character) override;
};

/**
 * @brief A power-up that increases a character's maximum bomb capacity.
 */
class BombPowerUp : public PowerUp {
public:
    /**
     * @brief Constructs a new BombPowerUp instance.
     * @param position The 2D world coordinates.
     * @param size The bounding box dimensions.
     */
    explicit BombPowerUp(Vector2 position, Vector2 size)
        : PowerUp(position, size, PowerUpType::ExtraBomb) {
    }

    /**
     * @brief Applies an extra bomb capacity increase to the given character.
     * @param character Reference to the character.
     */
    void applyTo(Character& character) override;
};

/**
 * @brief A power-up that increases a character's movement speed.
 */
class SkatesPowerUp : public PowerUp {
public:
    /**
     * @brief Constructs a new SkatesPowerUp instance.
     * @param position The 2D world coordinates.
     * @param size The bounding box dimensions.
     */
    explicit SkatesPowerUp(Vector2 position, Vector2 size)
        : PowerUp(position, size, PowerUpType::Skates) {
    }

    /**
     * @brief Applies a movement speed boost to the given character.
     * @param character Reference to the character.
     */
    void applyTo(Character& character) override;
};

} // namespace logic
