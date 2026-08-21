#pragma once

#include "representation/State.hpp"
#include <SFML/Audio.hpp>

namespace representation {
/**
 * @brief Represents the active gameplay screen state.
 *
 * Inherits from State to manage active world simulation updates, player input handling,
 * background tile rendering, HUD score tracking, and gameplay music.
 */
class PlayingState : public State {
public:
    using State::State;

    /**
     * @brief Constructs a new PlayingState instance.
     * @reference game Reference to the core Game application engine.
     */
    PlayingState(Game& game);

    /**
     * @brief Handles window and keyboard input events during active gameplay.
     * @param event The SFML event to process.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Updates the active game world simulation, animations, and scoring over time.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
    void update(double deltaTime) override;

    /**
     * @brief Renders the arena background, world entities, views, and HUD score text onto the window.
     * @reference window The SFML render window target.
     */
    void render(sf::RenderWindow& window) override;

private:
    sf::Texture backgroundTexture_;     ///< Texture loaded for tiling the arena background floor.
    sf::Sprite backgroundTile_;         ///< Sprite used to draw repeated arena background tiles.
    bool backgroundReady_ = false;      ///< Flag tracking whether background textures loaded successfully.
    sf::Font font_;                     ///< Font used for rendering HUD text elements.
    sf::Text scoreText_;                ///< Text displaying current score and high scores on screen.
    float aliveScoreAccumulator_ = 0.f; ///< Internal timer tracking points awarded over time for staying alive.
    sf::Music backgroundMusic_;         ///< SFML music stream playing gameplay background audio tracks.
};
} // namespace representation
