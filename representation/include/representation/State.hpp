#pragma once

#include <SFML/Graphics.hpp>

namespace representation {
/**
 * @brief Abstract base class representing an application state in the state machine.
 *
 * Defines the standard interface for handling window events, updating logic simulation,
 * and rendering screen content for distinct application phases (e.g., menu, gameplay, game over).
 */
class Game;
/**
 * @brief Constructs a new State instance.
 * @reference game Reference to the core Game application engine.
 */
class State {
public:
    /**
     * @brief Constructs a new State instance.
     * @reference game Reference to the core Game application engine.
     */
    explicit State(Game& game)
        : game_(game) {
    }

    /**
     * @brief Virtual destructor for the State.
     */
    virtual ~State() = default;

    /**
     * @brief Handles window and input events specific to the active state.
     * @param event The SFML event to process.
     */
    virtual void handleEvent(const sf::Event& event) = 0;

    /**
     * @brief Updates state-specific logic and animations over time.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
    virtual void update(double deltaTime) = 0;

    /**
     * @brief Renders state-specific UI, backgrounds, and entities onto the target window.
     * @reference window The SFML render window target.
     */
    virtual void render(sf::RenderWindow& window) = 0;

protected:
    Game& game_; ///< Reference to the core Game application engine.
};
} // namespace representation
