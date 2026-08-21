#pragma once

#include <memory>

#include "logic/Bomb.hpp"
#include "logic/Bot.hpp"
#include "logic/Character.hpp"
#include "logic/PowerUp.hpp"
#include "logic/Vector2.hpp"
#include "logic/Wall.hpp"
#include "logic/Flame.hpp"

namespace logic {
    /**
 * @brief Abstract factory interface for creating game entities.
 *
 * Defines the interface for the Abstract Factory pattern, allowing the creation
 * of game objects without specifying their concrete classes.
 */
    class AbstractFactory {
    public:
        /**
     * @brief Virtual destructor for the AbstractFactory.
     */
        virtual ~AbstractFactory() = default;

        /**
     * @brief Creates a player character instance.
     *
     * @param position The initial 2D world coordinates for the player.
     * @param size The bounding box dimensions for the player.
     * @return A shared pointer to the newly created Character instance.
     */
        virtual std::shared_ptr<Character> createPlayer(Vector2 position, Vector2 size) = 0;

        /**
     * @brief Creates an AI-controlled bot character instance.
     *
     * @param position The initial 2D world coordinates for the bot.
     * @param size The bounding box dimensions for the bot.
     * @return A shared pointer to the newly created Bot instance.
     */
        virtual std::shared_ptr<Bot> createBot(Vector2 position, Vector2 size) = 0;

        /**
     * @brief Creates a bomb instance placed in the game world.
     *
     * @param position The 2D world coordinates where the bomb is planted.
     * @param size The bounding box dimensions for the bomb.
     * @param radius The explosion radius of the bomb in tiles.
     * @param owner A weak reference to the character who planted the bomb.
     * @return A shared pointer to the newly created Bomb instance.
     */
        virtual std::shared_ptr<Bomb> createBomb(Vector2 position, Vector2 size, int radius,
                                                 std::weak_ptr<Character> owner) = 0;

        /**
     * @brief Creates a wall obstacle in the game world.
     *
     * @param position The 2D world coordinates for the wall.
     * @param size The bounding box dimensions for the wall.
     * @param destructible Flag indicating whether the wall can be destroyed by explosions.
     * @return A shared pointer to the newly created Wall instance.
     */
        virtual std::shared_ptr<Wall> createWall(Vector2 position, Vector2 size,
                                                 bool destructible) = 0;

        /**
     * @brief Creates a collectible power-up item.
     *
     * @param position The 2D world coordinates for the power-up.
     * @param size The bounding box dimensions for the power-up.
     * @param type The specific category or effect of the power-up.
     * @return A shared pointer to the newly created PowerUp instance.
     */
        virtual std::shared_ptr<PowerUp> createPowerUp(Vector2 position, Vector2 size,
                                                       PowerUpType type) = 0;

        /**
     * @brief Creates an explosion flame segment.
     *
     * @param position The 2D world coordinates for the flame segment.
     * @param size The bounding box dimensions for the flame.
     * @param part The specific section of the flame (e.g., center, tip, body).
     * @return A shared pointer to the newly created Flame instance.
     */
        virtual std::shared_ptr<Flame> createFlame(Vector2 position, Vector2 size, FlamePart part) = 0;
    };
} // namespace logic
