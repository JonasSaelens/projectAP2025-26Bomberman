#pragma once

#include "representation/State.hpp"
#include <SFML/Audio.hpp>

namespace representation {
/**
 * @brief Represents the main menu screen state of the application.
 *
 * Inherits from State to render the title screen, handle menu navigation inputs,
 * initiate game sessions, and play background menu music.
 */
class MenuState : public State {
public:
    using State::State;

    /**
     * @brief Constructs a new MenuState instance.
     * @reference game Reference to the core Game application engine.
     */
    MenuState(Game& game);

    /**
     * @brief Handles window and keyboard input events while on the main menu.
     * @param event The SFML event to process.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Updates the menu screen state over time.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
    void update(double deltaTime) override;

    /**
     * @brief Renders the menu UI elements, title, and buttons onto the window.
     * @reference window The SFML render window target.
     */
    void render(sf::RenderWindow& window) override;

private:
    sf::Font font_;             ///< Font used for rendering menu text elements.
    sf::Text titleText_;        ///< Game title banner text.
    sf::Text playButtonText_;   ///< Interactive prompt text to start playing.
    sf::Music backgroundMusic_; ///< SFML music stream playing background audio tracks.
};
} // namespace representation
