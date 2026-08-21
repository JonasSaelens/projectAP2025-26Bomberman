#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "logic/Score.hpp"
#include "logic/World.hpp"
#include "representation/Camera.hpp"
#include "representation/ConcreteFactory.hpp"
#include "representation/StateManager.hpp"

namespace representation {
    /**
 * @brief Core game engine application class managing the main loop, window, systems, and states.
 *
 * Coordinates the SFML render window, world simulation logic, scoring tracking,
 * rendering camera, state machine navigation, and entity factories.
 */
    class Game {
    public:
        /**
     * @brief Constructs a new Game application instance.
     *
     * @param windowWidth Initial width of the game window in pixels.
     * @param windowHeight Initial height of the game window in pixels.
     * @param title Title string displayed on the window title bar.
     */
        Game(unsigned windowWidth, unsigned windowHeight, const std::string &title);

        /**
     * @brief Executes the primary application execution loop (handling events, updates, and rendering).
     */
        void run();

        /**
     * @brief Retrieves the active SFML render window.
     * @return Reference to the sf::RenderWindow.
     */
        sf::RenderWindow &getWindow() { return *window_; }
        /**
     * @brief Retrieves the active game world simulation model.
     * @return Reference to the logic::World.
     */
        logic::World &getWorld() { return *world_; }
        /**
     * @brief Retrieves the game score and high score tracker.
     * @return Reference to the logic::Score tracker.
     */
        logic::Score &getScore() { return *score_; }
        /**
     * @brief Retrieves the camera handling world-to-screen coordinate transformations.
     * @return Reference to the Camera.
     */
        Camera &getCamera() { return camera_; }
        /**
     * @brief Retrieves the state manager controlling game screens (menu, playing, game over).
     * @return Reference to the StateManager.
     */
        StateManager &getStateManager() { return stateManager_; }
        /**
     * @brief Retrieves the concrete factory used for model and view instantiation.
     * @return A shared pointer to the ConcreteFactory.
     */
        std::shared_ptr<ConcreteFactory> getFactory() { return factory_; }

        /**
     * @brief Initializes or resets the game world and score for a fresh game session.
     */
        void startNewGame();

    private:
        /**
     * @brief Processes incoming window and input events.
     */
        void processInput();

        /**
     * @brief Updates the game simulation and active state for the current frame.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void update(double deltaTime);

        /**
     * @brief Renders the current frame onto the window.
     */
        void render();

        std::shared_ptr<sf::RenderWindow> window_; ///< The SFML render window display target.
        Camera camera_; ///< Camera handling world-to-screen pixel projections.
        std::shared_ptr<ConcreteFactory> factory_; ///< Factory instantiating logic models and visual views.
        std::unique_ptr<logic::World> world_; ///< Active game world simulation container.
        std::shared_ptr<logic::Score> score_; ///< Score and high score management instance.
        StateManager stateManager_; ///< State manager handling application screens and transitions.
    };
} // namespace representation
