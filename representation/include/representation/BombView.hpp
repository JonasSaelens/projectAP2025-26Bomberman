#pragma once

#include "representation/EntityView.hpp"
#include <SFML/Audio.hpp>

namespace representation {
    /**
 * @brief Handles the visual presentation and sound effects for a Bomb entity.
 *
 * Inherits from EntityView to render the bomb sprite, animate its pulsing fuse,
 * and play explosion audio upon detonation via observer notifications.
 */
    class BombView : public EntityView {
    public:
        /**
     * @brief Constructs a new BombView instance.
     *
     * @param texture Shared pointer to the SFML texture atlas.
     * @param initialRect The texture source rectangle for the bomb's base frame.
     * @param size The world-space dimensions of the bomb.
     */
        BombView(std::shared_ptr<sf::Texture> texture, const sf::IntRect &initialRect, logic::Vector2 size)
            : EntityView(std::move(texture)),
              baseRect_(initialRect) {
            sprite_.setTextureRect(baseRect_);
            setSize(size);

            if (soundBuffer_.loadFromFile("assets/bombexplode.wav")) {
                explodeSound_.setBuffer(soundBuffer_);
            }
        }

        /**
     * @brief Handles incoming game events to update the bomb's visual and audio state.
     * @param event The event details dispatched by the subject.
     */
        void onNotify(const logic::Event &event) override {
            EntityView::onNotify(event);

            switch (event.type) {
                case logic::EventType::Spawned:
                    sprite_.setTextureRect(baseRect_);
                    animTime_ = 0.0;
                    frame_ = 0;
                    break;

                case logic::EventType::BombExploded:
                    explodeSound_.play();
                    break;

                default:
                    break;
            }
        }

        /**
     * @brief Updates the bomb's sprite animation frame over time.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void updateAnimation(double deltaTime) {
            if (!alive_)
                return;

            animTime_ += deltaTime;
            frame_ = static_cast<int>(animTime_ / frameDuration_) % 3;

            const int frameW = baseRect_.width;
            const int frameH = baseRect_.height;
            sprite_.setTextureRect(sf::IntRect(
                baseRect_.left + frame_ * frameW,
                baseRect_.top,
                frameW,
                frameH
            ));
        }

    private:
        sf::IntRect baseRect_{}; ///< The initial texture coordinate region for the bomb sprite sheet.
        double animTime_ = 0.0; ///< Accumulated time tracking current animation progress.
        int frame_ = 0; ///< Index of the current active animation frame.
        double frameDuration_ = 0.12; ///< Duration each frame is displayed before switching.
        sf::SoundBuffer soundBuffer_; ///< Audio buffer holding the explosion sound data.
        sf::Sound explodeSound_; ///< SFML sound instance for playing the explosion effect.
    };
} // namespace representation
