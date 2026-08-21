#pragma once

#include "representation/State.hpp"
#include <SFML/Audio.hpp>

namespace representation {
/**
 * @brief Represents the game over screen state (handling victory or defeat outcomes).
 *
 * Inherits from State to display final match results, accumulated scores,
 * win/loss notification sounds, and options to return to the main menu or restart.
 */
class GameOverState : public State {
public:
    /**
     * @brief Constructs a new GameOverState instance.
     *
     * @reference game Reference to the core Game application engine.
     * @param hasWon Flag indicating whether the player won or lost the match.
     * @param finalScore The total score achieved by the player during the session.
     */
    GameOverState(Game& game, bool hasWon, int finalScore);

    /**
     * @brief Handles window and keyboard input events while on the game over screen.
     * @param event The SFML event to process.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Updates the game over screen state over time.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
    void update(double deltaTime) override;

    /**
     * @brief Renders the game over UI elements onto the window.
     * @reference window The SFML render window target.
     */
    void render(sf::RenderWindow& window) override;

private:
    bool hasWon_;    ///< Flag tracking whether the player achieved victory.
    int finalScore_; ///< The final score recorded for the session.

    sf::Font font_;               ///< Font used for rendering screen text.
    sf::Text titleText_;          ///< Text banner displaying win or loss status.
    sf::Text scoreText_;          ///< Text displaying the final score summary.
    sf::Text instructionText_;    ///< Text displaying navigation prompts (e.g., press Enter/Esc).
    sf::SoundBuffer soundBuffer_; ///< Audio buffer holding the game over/victory sound effect.
    sf::Sound endSound_;          ///< SFML sound instance for playing the game outcome audio.
};
} // namespace representation
