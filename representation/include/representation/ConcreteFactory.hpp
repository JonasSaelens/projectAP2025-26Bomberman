#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

#include <vector>

#include "logic/AbstractFactory.hpp"
#include "representation/EntityView.hpp"

namespace representation {
    /**
 * @brief Concrete factory implementation of the AbstractFactory pattern for rendering and logic entities.
 *
 * Responsible for instantiating game model entities (player, bots, bombs, flames, walls, power-ups),
 * managing their corresponding visual views, handling texture caching, and attaching observers.
 */
    class ConcreteFactory : public logic::AbstractFactory {
    public:
        /**
     * @brief Constructs a new ConcreteFactory instance.
     * @param window Shared pointer to the SFML render window.
     */
        explicit ConcreteFactory(std::shared_ptr<sf::RenderWindow> window);

        /**
     * @brief Creates a human player model and its associated visual view.
     *
     * @param position Initial 2D world coordinates.
     * @param size Dimensions of the player.
     * @return A shared pointer to the created player Character.
     */
        std::shared_ptr<logic::Character> createPlayer(logic::Vector2 position,
                                                       logic::Vector2 size) override;

        /**
     * @brief Creates an AI bot character model and its associated visual view.
     *
     * @param position Initial 2D world coordinates.
     * @param size Dimensions of the bot.
     * @return A shared pointer to the created Bot character.
     */
        std::shared_ptr<logic::Bot> createBot(logic::Vector2 position,
                                              logic::Vector2 size) override;

        /**
     * @brief Creates a bomb model and its associated visual view.
     *
     * @param position Initial 2D world coordinates.
     * @param size Dimensions of the bomb.
     * @param radius Explosion radius of the bomb.
     * @param owner Weak pointer to the character who placed the bomb.
     * @return A shared pointer to the created Bomb.
     */
        std::shared_ptr<logic::Bomb> createBomb(logic::Vector2 position, logic::Vector2 size,
                                                int radius,
                                                std::weak_ptr<logic::Character> owner) override;

        /**
     * @brief Creates an explosion flame segment model and its associated visual view.
     *
     * @param position Initial 2D world coordinates.
     * @param size Dimensions of the flame segment.
     * @param part The specific geometric segment type of the flame.
     * @return A shared pointer to the created Flame.
     */
        std::shared_ptr<logic::Flame>
        createFlame(logic::Vector2 position, logic::Vector2 size, logic::FlamePart part) override;

        /**
     * @brief Creates a wall or block model and its associated visual view.
     *
     * @param position Initial 2D world coordinates.
     * @param size Dimensions of the wall.
     * @param destructible Flag indicating if the wall can be destroyed by explosions.
     * @return A shared pointer to the created Wall.
     */
        std::shared_ptr<logic::Wall> createWall(logic::Vector2 position, logic::Vector2 size,
                                                bool destructible) override;

        /**
     * @brief Creates a power-up model and its associated visual view.
     *
     * @param position Initial 2D world coordinates.
     * @param size Dimensions of the power-up.
     * @param type The specific bonus category of the power-up.
     * @return A shared pointer to the created PowerUp.
     */
        std::shared_ptr<logic::PowerUp> createPowerUp(logic::Vector2 position, logic::Vector2 size,
                                                      logic::PowerUpType type) override;

        /**
     * @brief Retrieves all active entity views managed by the factory.
     * @return A constant reference to a vector of shared entity view pointers.
     */
        const std::vector<std::shared_ptr<EntityView> > &getViews() const { return views_; }

    private:
        /**
     * @brief Loads a texture from file or retrieves it from the cache if already loaded.
     *
     * @param path Filepath to the texture asset.
     * @return A shared pointer to the loaded SFML texture.
     */
        std::shared_ptr<sf::Texture> loadTexture(const std::string &path);

        std::shared_ptr<sf::RenderWindow> window_; ///< Target rendering window.
        std::map<std::string, std::shared_ptr<sf::Texture> > textureCache_;
        ///< Cache preventing redundant texture reloads.
        std::vector<std::shared_ptr<EntityView> > views_; ///< Collection of all active visual entity views.
        std::shared_ptr<logic::Observer> scoreObserver_; ///< Observer handling scoring logic for created elements.
        std::vector<int> botColumns_ = {8, 0, 8}; ///< Texture sheet column offsets for distinct bot skins.
        std::vector<int> botRows_ = {0, 11, 11}; ///< Texture sheet row offsets for distinct bot skins.
        std::vector<bool> botUsed_ = {false, false, false}; ///< Tracking flags for assigned bot skin variations.
    };
} // namespace representation
