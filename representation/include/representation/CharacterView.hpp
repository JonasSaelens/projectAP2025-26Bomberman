#pragma once

#include <SFML/Graphics.hpp>

#include "logic/Direction.hpp"
#include "logic/Event.hpp"
#include "representation/EntityView.hpp"

namespace representation {
    /**
 * @brief Defines the animation states for a character sprite.
 */
    enum class AnimState {
        Idle,
        Walk,
        Death
    };

    /**
 * @brief Handles the visual representation, skin customization, and animations for a Character entity.
 *
 * Manages sprite sheets, direction-based facing, walking frames, death sequences,
 * and custom skin offsets via observer notifications.
 */
    class CharacterView : public EntityView {
    public:
        /**
     * @brief Constructs a new CharacterView instance.
     *
     * @param texture Shared pointer to the character texture sheet.
     * @param size The world-space dimensions of the character.
     * @param skinColOffset Column offset on the sprite sheet for custom skins.
     * @param skinRowOffset Row offset on the sprite sheet for custom skins.
     */
        CharacterView(std::shared_ptr<sf::Texture> texture, logic::Vector2 size,
                      int skinColOffset = 0, int skinRowOffset = 0)
            : EntityView(std::move(texture)),
              skinColOffset_(skinColOffset),
              skinRowOffset_(skinRowOffset) {
            setSize(size);
        }

        /**
     * @brief Handles incoming game events to update the character's animation state and facing direction.
     * @param event The event details dispatched by the subject.
     */
        void onNotify(const logic::Event &event) override {
            switch (event.type) {
                case logic::EventType::Died:
                    moving_ = false;
                    frame_ = 0;
                    animationTime_ = 0.0;
                    animState_ = AnimState::Death;
                    deathFrame_ = 0;
                    deathTime_ = 0.0;
                    return;
                default:
                    EntityView::onNotify(event);
                    break;
            }

            switch (event.type) {
                case logic::EventType::Spawned:
                    facing_ = event.facing;
                    moving_ = false;
                    frame_ = 0;
                    animationTime_ = 0.0;
                    break;

                case logic::EventType::Moved:
                    if (event.facing == logic::Direction::None) {
                        moving_ = false;
                        frame_ = 0;
                        animationTime_ = 0.0;
                    } else {
                        facing_ = event.facing;
                        moving_ = true;
                    }
                    break;

                default:
                    break;
            }
        }

        /**
     * @brief Updates walking or death animation frames over time.
     * @param deltaTime The time elapsed since the last update frame in seconds.
     */
        void updateAnimation(double deltaTime) {
            if (animState_ == AnimState::Death) {
                deathTime_ += deltaTime;
                if (deathTime_ >= deathFrameDuration_) {
                    deathTime_ -= deathFrameDuration_;
                    ++deathFrame_;
                    if (deathFrame_ >= deathFrameCount) {
                        alive_ = false;
                    }
                }
                return;
            }

            if (!moving_) {
                frame_ = 0;
                animationTime_ = 0.0;
                return;
            }

            animationTime_ += deltaTime;
            if (animationTime_ >= frameDuration_) {
                animationTime_ -= frameDuration_;
                frame_ = (frame_ + 1) % 3;
            }
        }

        /**
     * @brief Renders the character sprite onto the SFML window based on current state and facing direction.
     *
     * @reference window The SFML render window target.
     * @reference camera The camera handling world-to-screen coordinate mapping.
     */
        void draw(sf::RenderWindow &window, const Camera &camera) override {
            if (!alive_) return;

            constexpr int tileW = 32;
            constexpr int tileH = 32;

            int row = 1;
            int col = 0;
            bool mirror = false;

            if (animState_ == AnimState::Death) {
                row = 8;
                col = 5 + deathFrame_;
            } else {
                switch (facing_) {
                    case logic::Direction::Down:
                        row = 1;
                        col = frame_;
                        break;
                    case logic::Direction::Up:
                        row = 2;
                        col = frame_;
                        break;
                    case logic::Direction::Right:
                        row = 1;
                        col = 3 + frame_;
                        mirror = false;
                        break;
                    case logic::Direction::Left:
                        row = 1;
                        col = 3 + frame_;
                        mirror = true;
                        break;
                    case logic::Direction::None:
                        row = 1;
                        col = 0;
                        break;
                }
            }

            int finalCol = col;
            int finalRow = row;

            if (animState_ != AnimState::Death) {
                finalCol += skinColOffset_;
                finalRow += skinRowOffset_;
            } else {
                if (skinRowOffset_ > 0 || skinColOffset_ > 0) {
                    finalRow += 1;
                }
            }

            sprite_.setTextureRect(sf::IntRect(finalCol * tileW, finalRow * tileH, tileW, tileH));

            const sf::Vector2f targetPx = camera.worldToScreenScale(size_);
            const float sx = targetPx.x / static_cast<float>(tileW);
            const float sy = targetPx.y / static_cast<float>(tileH);

            sprite_.setOrigin(tileW * 0.5f, tileH * 0.5f);
            sprite_.setScale(mirror ? -sx : sx, sy);
            sprite_.setPosition(camera.worldToScreen(lastKnownPosition_));

            window.draw(sprite_);
        }

    private:
        logic::Direction facing_ = logic::Direction::Down; ///< Current facing direction of the character.
        int frame_ = 0; ///< Current animation frame index.
        double animationTime_ = 0.0; ///< Accumulated time tracking walking animation progress.
        double frameDuration_ = 0.15; ///< Duration each walk frame is displayed.
        bool moving_ = false; ///< Flag indicating if the character is currently walking.

        int skinColOffset_ = 0; ///< Texture sheet column offset for custom skins.
        int skinRowOffset_ = 0; ///< Texture sheet row offset for custom skins.

        AnimState animState_ = AnimState::Idle; ///< Active high-level animation state.
        int deathFrame_ = 0; ///< Current frame index of the death sequence.
        double deathTime_ = 0.0; ///< Accumulated time tracking death animation progress.
        double deathFrameDuration_ = 0.12; ///< Duration each death frame is displayed.
        static constexpr int deathFrameCount = 2; ///< Total number of frames in the death sequence.
    };
} // namespace representation