#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "logic/Observer.hpp"
#include "representation/Camera.hpp"

namespace representation {
    /**
 * @brief Abstract base class for rendering game entities and handling visual observer events.
 *
 * Inherits from logic::Observer to track entity movements, spawns, and death states,
 * automatically updating sprite transformations and rendering them onto the screen.
 */
    class EntityView : public logic::Observer {
    public:
        /**
     * @brief Constructs a new EntityView instance.
     * @param texture Shared pointer to the SFML texture used for rendering the sprite.
     */
        explicit EntityView(std::shared_ptr<sf::Texture> texture) : texture_(std::move(texture)) {
            sprite_.setTexture(*texture_);
        }

        /**
     * @brief Virtual destructor for the EntityView.
     */
        ~EntityView() override = default;

        /**
     * @brief Sets the source texture rectangle region for the sprite.
     * @param rect The sf::IntRect texture coordinates.
     */
        void setTextureRect(const sf::IntRect &rect) {
            sprite_.setTextureRect(rect);
        }

        /**
     * @brief Sets the rendering layer order for depth sorting.
     * @param layer Integer layer value (higher values render on top).
     */
        void setLayer(int layer) {
            layer_ = layer;
        }

        /**
     * @brief Gets the current rendering layer order.
     * @return The integer layer value.
     */
        int getLayer() const {
            return layer_;
        }

        /**
     * @brief Callback method handling incoming game events to update position or liveness.
     * @param event The event details dispatched by the subject.
     */
        void onNotify(const logic::Event &event) override {
            switch (event.type) {
                case logic::EventType::Moved:
                case logic::EventType::Spawned:
                    lastKnownPosition_ = event.position;
                    break;
                case logic::EventType::Died:
                    alive_ = false;
                    break;
                default:
                    break;
            }
        }

        /**
     * @brief Sets the world-space dimensions of the entity view.
     * @param size The Vector2 size dimensions.
     */
        void setSize(logic::Vector2 size) {
            size_ = size;
        }

        /**
     * @brief Renders the entity sprite onto the window using camera coordinate transformations.
     *
     * @reference window The SFML render window target.
     * @reference camera The camera handling world-to-screen scaling and positioning.
     */
        virtual void draw(sf::RenderWindow &window, const Camera &camera) {
            if (!alive_) return;

            const sf::Vector2f targetPx = camera.worldToScreenScale(size_);

            sf::IntRect rect = sprite_.getTextureRect();
            if (rect.width == 0 || rect.height == 0) {
                const sf::Vector2u texSize = texture_->getSize();
                rect = sf::IntRect(0, 0, static_cast<int>(texSize.x), static_cast<int>(texSize.y));
            }

            const float sx = targetPx.x / static_cast<float>(rect.width);
            const float sy = targetPx.y / static_cast<float>(rect.height);

            sprite_.setOrigin(rect.width * 0.5f, rect.height * 0.5f);
            sprite_.setScale(sx, sy);
            sprite_.setPosition(camera.worldToScreen(lastKnownPosition_));

            window.draw(sprite_);
        }

    protected:
        sf::Sprite sprite_; ///< SFML sprite used for rendering.
        std::shared_ptr<sf::Texture> texture_; ///< The underlying texture resource.
        logic::Vector2 lastKnownPosition_{}; ///< Last recorded world position of the entity.
        bool alive_ = true; ///< Flag tracking whether the view should remain active.
        logic::Vector2 size_{1.0, 1.0}; ///< World-space dimensions of the entity.
        int layer_ = 0; ///< Rendering depth sorting layer.
    };
} // namespace representation
