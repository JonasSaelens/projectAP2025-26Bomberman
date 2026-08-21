#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "logic/AbstractFactory.hpp"
#include "logic/Bomb.hpp"
#include "logic/Bot.hpp"
#include "logic/Character.hpp"
#include "logic/Direction.hpp"
#include "logic/EntityModel.hpp"
#include "logic/Wall.hpp"

namespace logic {
    /**
 * @brief Manages the game state, entities, arena initialization, and update loops.
 *
 * Acts as the central game world container, handling movement collision checks,
 * bomb detonations, entity lifecycles, and win/loss conditions.
 */
    class World {
    public:
        static constexpr int kArenaRows = 13; ///< The total number of rows in the game arena.
        static constexpr int kArenaCols = 15; ///< The total number of columns in the game arena.
        static constexpr double kPowerUpDropChance = 0.25;
        ///< Probability that a power-up spawns when destroying a block.
        mutable std::mutex g_worldMutex; ///< Mutex ensuring thread-safe access to world entities.

        /**
     * @brief Constructs a new World instance.
     * @param factory Shared pointer to the AbstractFactory used for entity creation.
     */
        explicit World(std::shared_ptr<AbstractFactory> factory);

        /**
     * @brief Initializes the arena layout, walls, player, and bots.
     */
        void initializeArena();

        /**
     * @brief Updates the entire game world state for the current frame.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void update(double deltaTime);

        /**
     * @brief Requests a bomb to be placed by a specific character.
     * @param owner Pointer to the character planting the bomb.
     */
        void requestPlaceBomb(Character *owner);

        /**
     * @brief Attempts to move a character in a given direction, handling collisions.
     *
     * @param character Reference to the character attempting to move.
     * @param direction The movement direction.
     * @param deltaTime The frame time delta.
     * @return True if movement was successful, false if blocked.
     */
        bool moveIfPossible(Character &character, Direction direction, double deltaTime);

        /**
     * @brief Sets the pending movement direction for the human player.
     * @param direction The desired movement Direction.
     */
        void setPlayerDirection(Direction direction) { pendingPlayerDirection_ = direction; }
        /**
     * @brief Signals that the player has requested to place a bomb.
     */
        void requestPlayerPlaceBomb() { playerWantsToPlaceBomb_ = true; }

        /**
     * @brief Retrieves the human player character instance.
     * @return A shared pointer to the player Character.
     */
        std::shared_ptr<Character> getPlayer() const { return player_; }
        /**
     * @brief Gets all active entities currently in the world.
     * @return A constant reference to a vector of shared entity pointers.
     */
        const std::vector<std::shared_ptr<EntityModel> > &getEntities() const { return entities_; }

        /**
     * @brief Checks whether the game has concluded (win or loss).
     * @return True if the game is over, false otherwise.
     */
        bool isGameOver() const { return gameOver_; }
        /**
     * @brief Checks whether the human player has won the game.
     * @return True if the player won, false otherwise.
     */
        bool didPlayerWin() const { return playerWon_; }

        /**
     * @brief Checks if a moving entity would collide with solid obstacles at a target position.
     *
     * @const mover Reference to the entity attempting to move.
     * @param nextPosition The target 2D world coordinates to test.
     * @return True if a collision would occur, false if clear.
     */
        bool wouldCollideWithSolid(const EntityModel &mover, Vector2 nextPosition) const;

        /**
     * @brief Thread-safe retrieval of all entities in the world.
     * @return A vector copy of all active entity pointers.
     */
        std::vector<std::shared_ptr<EntityModel> > getEntitiesSafe() const {
            std::lock_guard<std::mutex> lock(g_worldMutex);
            return entities_;
        }

        /**
     * @brief Checks if the player has won the match.
     * @return True if player won, false otherwise.
     */
        bool has_player_won() const {
            return playerWon_;
        }

    private:
        /**
     * @brief Spawns a bomb in the world for the specified character.
     * @param owner Shared pointer to the character placing the bomb.
     */
        void placeBomb(const std::shared_ptr<Character> &owner);

        /**
     * @brief Resolves an explosion triggered by a detonated bomb, generating flames and affecting entities.
     * @reference bomb Reference to the exploding Bomb.
     */
        void resolveExplosion(Bomb &bomb);

        /**
     * @brief Cleans up and removes dead or expired entities from the world container.
     */
        void removeDeadEntities();

        std::shared_ptr<AbstractFactory> factory_; ///< Factory used to instantiate world elements.
        std::vector<std::shared_ptr<EntityModel> > entities_; ///< Container of all active world entities.
        std::shared_ptr<Character> player_; ///< Reference to the human player character.
        std::vector<std::shared_ptr<Bot> > bots_; ///< Collection of active AI bot characters.
        std::vector<std::shared_ptr<Flame> > flames_; ///< Collection of active explosion flame segments.

        Direction pendingPlayerDirection_ = Direction::None; ///< Next queued direction input for the player.
        bool playerWantsToPlaceBomb_ = false; ///< Flag tracking if the player requested a bomb drop.

        bool gameOver_ = false; ///< Flag indicating if the game session has ended.
        bool playerWon_ = false; ///< Flag tracking whether the player achieved victory.
    };
} // namespace logic
