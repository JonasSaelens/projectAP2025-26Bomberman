#pragma once

#include "logic/Character.hpp"

namespace logic {

/**
 * @brief Represents an AI-controlled bot character in the game.
 *
 * Extends the base Character class to include decision-making capabilities,
 * pathfinding state, and autonomous behavior within the game world.
 */
class Bot : public Character {
public:
    using Character::Character;

    /**
     * @brief Determines and executes the bot's next action based on the current world state.
     *
     * Analyzes the environment (such as nearby threats, targets, or paths) and
     * updates the bot's behavior for the current frame.
     *
     * @reference World Reference to the current game world state.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
    void decideAction(World& world, double deltaTime);

private:
    std::vector<std::pair<int, int> > path_; ///< The current grid-based movement path for the bot.
    bool hasTarget_ = false;                 ///< Flag indicating whether the bot currently has an active target.
};

} // namespace logic
